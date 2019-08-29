#include "library.h"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <pthread.h>
#include <semaphore.h>

#include "ConcurrentStaticQueue.h"
#include "Thread.h"
#include "ThreadPool.h"

using namespace std;


void hello()
{
    std::cout << "Hello, World!" << std::endl;
}

void printStr(const char* str)
{
    printf("%s\n", str);
}

std::string makeStr()
{
    return "made this string";
}

void takeStr(std::string str)
{
    std::cout << str << '\n';
}

void changeStr(char* str, int length)
{
    cout << str << " " << length << '\n';

    for (int i = 0; i < length; i++) {
        str[i] = 'a';
    }

    printf("changed str: %s\n", str);
}

void fillArray(int arr[], int n)
{
    for (int i = 0; i < n; i++) {
        arr[i] += i;
    }
}

void changeIntPointerArray(int* array, int n)
{
    for (int i = 0; i < n; i++)
        array[i] = i * 2;
}

uint64_t fib(uint64_t i)
{
    if (i < 1) return 0;
    if (i == 1) return 1;
    if (i == 2) return 2;

    return fib(i-1) + fib(i-2);
}

void printFib(int i)
{
    printf("fib: %lu\n", fib(i));
}

vector<int> testVec = {1,2,3,4,5,6,7,8,9,10};

vector<thread> tVec;

thread* createNewThread(int i)
{
    return new thread(printFib, i);
}

void waitThread(std::thread* t)
{
    t->join();
}

mutex m;
condition_variable cv;
int i = 0;
int nReleased = 0;

void printNum()
{
    for (;;)
    {
        unique_lock<mutex> lk(m);

        cerr << "waiting...\n";

        cv.wait(lk, [] { return nReleased > 0; });
        nReleased--;

        if (i > testVec.size()) {
            printf("index %d: out of range\n", i);
            return;
        }
        printf("index %d: %d\n", i, testVec[i]);
        i++;
        m.unlock();
    }
}

void initThreads(int numThreads)
{
    for (int i = 0; i < numThreads; i++)
        tVec.emplace_back(printNum);
}

void getResult(int n)
{
    m.lock();
    nReleased += n;
    m.unlock();

    cv.notify_all();
}

class CSQueueProducerThread : public Thread {
    CSQueue<int>& dataQueue_;

    inline static constexpr int TOTAL_TASKS = 100;

    void internalRun() final
    {
        for (int i = 0; i < TOTAL_TASKS; i++)
        {
            this_thread::sleep_for(chrono::milliseconds(10));

            dataQueue_.push(i);
        }

        dataQueue_.setQuitSignal();
    }

public:
    explicit CSQueueProducerThread(CSQueue<int>& dataQueue) : dataQueue_(dataQueue) { }
};

constexpr int MAX_SIZE = 100;
constexpr int WRITE_AHEAD = 50;
constexpr int N_CONSUMERS = 20;

CSQueue<int> dataQueue(MAX_SIZE, WRITE_AHEAD, N_CONSUMERS);
CSQueueProducerThread producerThread(dataQueue);

void startProducerThread()
{
    producerThread.run();
}

bool getResultChunk(vector<int>* chunk, int n)
{
    chunk->reserve(n);
    for (int i = 0; i < n; i++)
    {
        auto [res, quit] = dataQueue.pop();

        if (quit) return true;

        chunk->push_back(res);
    }
    return false;
}

void waitProducerThread()
{
    producerThread.wait();
}


DatabaseQueryManager::DatabaseQueryManager(
        const char* indexFilename,
        const char* dataPath,
        const char* dataFilePrefix) :

        indexFilename_(indexFilename),
        dataPath_(dataPath),
        dataFilePrefix_(dataFilePrefix),
        indexTable_({IndexTable::PID, IndexTable::AID}),
        pidTable_(indexTable_[IndexTable::PID]),
        aidTable_(indexTable_[IndexTable::AID])
{
    indexTable_.readFromFile(indexFilename);
}

void DatabaseQueryManager::getAllId(vector<string>* pidList, vector<string>* aidList) const
{
    if (pidList != nullptr) {
        pidList->reserve(pidTable_.size());
        for (const auto& [pid, _] : pidTable_)
            pidList->push_back(pid);
    }
    if (aidList != nullptr) {
        aidList->reserve(aidTable_.size());
        for (const auto& [aid, _] : aidTable_)
            aidList->push_back(aid);
    }
}

bool DatabaseQueryManager::getContentById(const char* id, DataRecord* dataRecord) const
{
    const IndexValue* iv;
    if (!(iv = getInfoById(id))) {
        fprintf(stderr, "%s: ID [%s] does not exist in database\n",
                __FUNCTION__, id);
        return false;
    }

    std::string binFilename = getBinFilenameWithBinId(iv->binId);

    DataRecordFile dataRecordFile;
    dataRecordFile.readFromFile(binFilename.c_str());

    return dataRecordFile.GetDataRecordAtOffset(iv->offset, dataRecord);
}

const IndexValue* DatabaseQueryManager::getInfoById(const char* id) const
{

    if (pidTable_.find(id) != pidTable_.end()) {
        return pidTable_.at(id);
    }
    if (aidTable_.find(id) != aidTable_.end()) {
        return aidTable_.at(id);
    }
    // id not found
    fprintf(stderr, "%s: ID [%s] does not exist in database\n",
            __FUNCTION__, id);

    return nullptr;
}

void DatabaseQueryManager::getInfoByIdList(
        const std::vector<std::string>& idList, std::vector<const IndexValue*>* output) const
{
    for (const auto& id : idList) {
        const IndexValue* iv;
        if (!(iv = getInfoById(id.c_str())))
            continue;
        output->push_back(iv);
    }
}

void DatabaseQueryManager::getContentByIdList(const vector<string>& idList,
                                              vector<shared_ptr<IdDataRecord>>* idDataRecordList) const
{
    vector<const IndexValue*> infoList;
    getInfoByIdList(idList, &infoList);

    if (infoList.empty()) {
        fprintf(stderr, "%s: no IDs found\n", __FUNCTION__);
        return;
    }
    /* get all binId of data files needed to be opened */
    std::unordered_map<uint32_t, std::vector<const IndexValue*>> indexByBinId;
    for (const IndexValue* info : infoList) {
        indexByBinId[info->binId].push_back(info);
    }
//    idDataRecordList->reserve(idList.size());

    // TODO: optimize for caching
    for (const auto& [binId, ivList]: indexByBinId) {
        DataRecordFile dataFile;
        std::string binName = getBinFilenameWithBinId(binId);
        dataFile.readFromFile(binName.c_str());
        /* read whole request if more than 1/3 of records must be accessed  */
        if (ivList.size() > dataFile.numRecords() / 3)
            dataFile.readFromFileFull(binName.c_str());

        for (const IndexValue* iv : ivList) {
            idDataRecordList->emplace_back(new IdDataRecord);
            if (!dataFile.GetDataRecordAtOffset(iv->offset,
                                                &idDataRecordList->back()->dataRecord))
                continue;
            idDataRecordList->back()->pid = iv->pid;
            idDataRecordList->back()->aid = iv->aid;
        }
    }
}
