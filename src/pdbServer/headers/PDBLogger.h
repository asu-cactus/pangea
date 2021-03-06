
/*
 * File:   PDBLogger.h
 * Author: Chris
 *
 * Created on September 26, 2015, 7:39 AM
 */

#ifndef PDBLOGGER_H
#define PDBLOGGER_H

#include <memory>
#include "LogLevel.h"

#include <pthread.h>


// used to log client and server activity to a text file

namespace pdb {

// create a smart pointer for PDBLogger objects
class PDBLogger;
typedef std::shared_ptr<PDBLogger> PDBLoggerPtr;

class PDBLogger {
public:
    // opens up a logger; output is written to the specified file
    PDBLogger(std::string fName);

    // opens up the logger
    void open(std::string fName);

    // JiaNote: why we need an empty logger here?
    // empty logger
    // PDBLogger();

    // closes the text file
    ~PDBLogger();

    // added by Jia, so that we can disable debug for performance testing
    void setEnabled(bool enabled);

    // writes a line of text to the log file
    void writeLn(std::string writeMe);

    // write data, added by Jia
    void write(char* data, unsigned int length);

    // write int, added by Jia
    void writeInt(int writeMe);

    LogLevel getLoglevel();

    void setLoglevel(LogLevel loglevel);

    // Log Levels are:
    //
    //	OFF,
    //	FATAL,
    //	ERROR,
    //	WARN,
    //	INFO,
    //	DEBUG,
    //	TRACE

    // writes a line of text to the log file, if log level FATAL, ERROR, WARN, INFO, DEBUG or TRACE
    // is activated
    void fatal(std::string writeMe);

    // writes a line of text to the log file, if log level ERROR, WARN, INFO, DEBUG or TRACE is
    // activated
    void error(std::string writeMe);

    // writes a line of text to the log file, if log level WARN, INFO, DEBUG or TRACE is activated
    void warn(std::string writeMe);

    // writes a line of text to the log file, if log level 	INFO, DEBUG or TRACE is activated
    void info(std::string writeMe);

    // writes a line of text to the log file, if log level 	DEBUG or TRACE is activated
    void debug(std::string writeMe);

    // writes a line of text to the log file, if log level 	TRACE is activated
    void trace(std::string writeMe);

private:
    // prohibits two people from writing to the file at the same time
    pthread_mutex_t fileLock;

    // the location we are writing to
    FILE* outputFile;

    bool enabled = true;

    LogLevel loglevel = WARN;
};
}

#endif /* PDBLOGGER_H */
