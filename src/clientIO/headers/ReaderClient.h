#ifndef READER_CLIENT_H
#define READER_CLIENT_H

#include "PDBClient.h"

#include <queue>
#include <string>
#include <mutex>
#include <thread>
#include <functional>

template <typename T>
class ReaderClient {
private:
    std::mutex *inMutex;
    std::mutex *countMutex;
    long currRowCount;
    bool maxFlag;
protected:
    int port;
    std::string managerIp;
    pdb::PDBLoggerPtr clientLogger;
    std::string objectToRegister;
    long maxRowCount = LONG_MAX;
  
public:
    ReaderClient(int port, std::string managerIp,
    pdb::PDBLoggerPtr clientLogger, long total, std::string objectPath){
        this->port = port;
        this->managerIp = managerIp;
        this->clientLogger = clientLogger;
        this->maxRowCount = total;
        this->objectToRegister = objectPath;
        this->inMutex = new std::mutex();
        this->countMutex = new std::mutex();
        this->currRowCount = 0;
        this->maxFlag = false;
    }

    ~ReaderClient(){
        delete this->countMutex;
        delete this->inMutex;
    }

    /**
     * Method to spawn the requested number of threads to load
     * a set of files.
     * @param numOfThreads Number of concurrent files to process
     * @param inFiles Referece to the queue of input streams
     * @param dbName Database name
     * @param setName Name of the data-set
     * @param blockSizeInMB Block of data to be sent for each
     * communication to the Dispatcher.
     */
    void exec(int numOfThreads, std::queue<std::ifstream *> &inFiles,
    std::string dbName, std::string setName, int blockSizeInMB){
    std::vector<std::thread> threadVec;
        for(int i = 0; i < numOfThreads; i++){
            threadVec.push_back(std::thread(readerRoutine, std::cref(inFiles), 
                dbName, setName, blockSizeInMB, this->port, this->managerIp,
                this->clientLogger, this->objectToRegister ,this->inMutex,
                this->countMutex, this->maxRowCount,
                std::cref(this->currRowCount), std::cref(this->maxFlag)
                std::ref(&(this->rowParser))));
            cout << "READER CLIENT: Thread " << i << " started."
                << std::endl;
        }
        for (int i = 0; i < numOfThreads; i++){
            threadVec.at(i).join();
            std::cout << "READER CLIENT: Thread " << i << " joined."
                << std::endl;
        }
    }

protected:
    /**
     * Method to parse each row from the input file.
     * 
     * @param line String input for the file reader for each row.
     * @param currCount Index to which current row is sent.
     */
    pdb::Handle<T> rowParser(std::string line, int currCount);

private:
    /**
     * Routine to execute for each input file stream.
     * 
     * This method will be called inside every thread,
     * while each thread reading a part file at a time.
     * 
     * @param inFiles Referece to the queue of input streams
     * @param dbName Database name
     * @param setName Name of the data-set
     * @param blockSizeInMB Block of data to be sent for each
     * @param port Port of the Server/Manager process
     * @param managerIp IP address of the Manager/Server/Driver
     * @param clientLogger PDB client logger pointer
     * @param objectToRegister PDB Object for the data to be loaded
     * @param inMutex Mutex to gaurd the File streams queue
     * @param countMutex Mutex to gaurd the count of rows loaded
     * @param maxRowCount Maximum rows to load
     * @param maxFlag Flag to check if the max row count is reached
     * @param parseRow Function to parse each line into row object
     * communication to the Dispatcher.
     */
    static void readerRoutine(std::queue<std::ifstream *> const &inFiles,
    std::string dbName, std::string setName, int blockSizeInMB, int port, 
    std::string managerIp, pdb::PDBLoggerPtr clientLogger,
    std::string objectToRegister, std::mutex *inMutex, std::mutex *countMutex,
    int maxRowCount, int const &rowCount, bool const &flag,
    std::function<pdb::Handle<T>(std::string, int)> &parserFunc){
        int &count = const_cast<int &>(rowCount);
        bool &stopFlag = const_cast<bool &>(flag);
        std::queue<std::ifstream *> &inFileQ = 
            const_cast<std::queue<std::ifstream *> &>(inFiles);
        std :: string line;
        bool rollback = false;
        bool end = false;
        string errMsg = "Error occurred in the reading client threads.";
        pdb::PDBClient pdbClient(port, managerIp, clientLogger, false, true);
        std::cout << "Registering Object " << objectToRegister << std::endl; 
        pdbClient.registerType(objectToRegister, errMsg);
        // Keep reading the files until all the files are read
        while(!inFiles.empty()){
            inMutex->lock();
            std::ifstream *currFile = inFileQ.front();
            inFileQ.pop();
            inMutex->unlock();
            rollback = false;
            end = false;
            pdb::makeObjectAllocatorBlock(
                (size_t)blockSizeInMB * (size_t)1024 * (size_t)1024, true);
            pdb::Handle<pdb::Vector<pdb::Handle<T>>> storeMe = 
                pdb::makeObject<pdb::Vector<pdb::Handle<T>>>();
            while (!end) {
                // Roll back one line, hence skip reading new line from stream.
                // And load the previously fetched line.
                if (!rollback) {
                    if(!std::getline(*currFile, line)){
                        end = true;
                        if (!pdbClient.sendData<T>(
                                std::pair<std::string,
                                std::string>(setName, dbName),
                                storeMe, errMsg)){
                            std::cout << "Failed to send data to dispatcher server"
                                << std::endl;
                            return;
                        }
                        pdbClient.flushData (errMsg);
                        std::cout << "Dispatched " << storeMe->size()
                            << setName << std::endl;
                        break;
                    }
                }
                // Keep loading the local [[pdb::Handle]] with each line.
                rollback = false; 
                try {
                    pdb::Handle<T> row = parserFunc(line, count);
                    storeMe->push_back(row);
                }
                catch (pdb::NotEnoughSpace &n) {
                    if (! pdbClient.sendData<T>(
                        std::pair<std::string, std::string>(setName, dbName),
                        storeMe, errMsg
                        )) {
                        std::cout << "Failed to send data to dispatcher server"
                            << std::endl;
                        return;
                    }
                    std::cout << "Dispatched " << storeMe->size()
                        << setName << std::endl;
                    countMutex->lock();
                    count = count + storeMe->size();
                    countMutex->unlock();
                    std::cout << "sent " << count
                        << " objects in total" << std::endl;
                    // end the loading if the count 
                    if (count >= maxRowCount) { 
                        end = true;
                        stopFlag = true;
                    }
                    // Roll back one line if current row loading fails.
                    rollback = true; 
                    pdb::makeObjectAllocatorBlock(
                        (size_t)blockSizeInMB * (size_t)1024 * (size_t)1024,
                        true);
                    storeMe =
                        pdb::makeObject<
                            pdb::Vector<pdb::Handle<T>>
                        >();
                }
            }
            pdbClient.flushData(errMsg);
            // Stop reading more File streams, if max row count has been reached.
            if(stopFlag) break;
            delete currFile;
        }
    }
};

#endif