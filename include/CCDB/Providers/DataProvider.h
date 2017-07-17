#ifndef _DDataProvider_
#define _DDataProvider_

#include <string>
#include <vector>
#include <map>

#include "CCDB/Model/Assignment.h"
#include "CCDB/Model/ConstantsTypeTable.h"
#include "CCDB/Model/Directory.h"
#include "CCDB/Model/RunRange.h"
#include "CCDB/Model/Variation.h"
#include "CCDB/CCDBError.h"



/* @class DataProvider
 * This is the main base interface to the Providers class family. 
 * Each derived <Something>Provider, provides access to data from a specified data source
 * I.e. MySQLDataProvider works with MySQL database, SQLiteDataProvider works with SQLite
 *
 *==============================================
 *Low Level API
 *
 *                    ^ ^ ^  
 *                    | | |                            
 *  +------------------------------------------+
 *  | Data Model:  Assignment,  TypeTable, ... |    -   Data model is returned to user
 *  +------------------------------------------+        
 *                       ^                              
 *                       |                              
 *                       |                              
 *  +------------------------------------------+        
 *  |    DataProvider - Interface to database  |    -   User calls DataProvider functions to get data user needs
 *  +------------------------------------------+        
 *                       |                              
 *             +---------------------+                  
 *            /                       \                 
 *  +----------------+        +----------------+        
 *  |  MySQLProvider |        |  SQLiteProvider|    -   Classes inherited from DataProvider do actual queries to data sources
 *  +----------------+        +----------------+        
 *          |                          |                
 *  <================>        <================>        
 *  | MySQL Database |        |     SQLite     |    -   Data storages 
 *  <________________>        <________________>
 *
 *
 *
 *
 */
namespace ccdb
{

    class DataProvider
    {
    public:

        DataProvider();


        //----------------------------------------------------------------------------------------
        //  C O N N E C T I O N
        //----------------------------------------------------------------------------------------

        /**
         * @brief Connects to database using connection string
         *
         * Connects to database using connection string
         * connection string might be in form:
         * mysql://<username>:<password>@<mysql.address>:<port> <database>
         *
         * @param connectionString "mysql://<username>:<password>@<mysql.address>:<port> <database>"
         * @return true if connected
         */
        virtual bool Connect(std::string connectionString) = 0;

        /**
         * @brief closes connection to data
         */
        virtual void Disconnect()= 0;

        /**
         * @brief indicates ether the connection is open or not
         *
         * @return true if  connection is open
         */
        virtual bool IsConnected()=0;

        /** @brief Connection string that was used on last successful connect.
         *
         * Connection string that was used on last successful connect.
         * If no any successfull connects were done string::empty will be returned
         *
         * @return  Last connection string or string::empty() if no successfull connection was done
         */
        virtual std::string GetConnectionString();

        //----------------------------------------------------------------------------------------
        //  D I R E C T O R Y   M A N G E M E N T
        //----------------------------------------------------------------------------------------
#ifndef __GNUC__
#pragma region Directory managemend
#endif

        /** @brief Gets directory by its full path
        *
        * @param   Full path of the directory
        * @return DDirectory object if directory exists, NULL otherwise
        */
        virtual shared_ptr<Directory> GetDirectory(const std::string &path)=0;

        /** @brief return reference to root directory
         *
         * Root directory contains all other directories. It is not stored in any database
         *
         * @warning User should not delete this object
         *
         * @return   DDirectory object pointer
         */
        virtual std::shared_ptr<Directory> const GetRootDirectory();

        /** @brief Searches for directory
         *
         * Searches directories that matches the pattern
         * inside parent directory with path @see parentPath
         * or globally through all type tables if parentPath is empty
         * The pattern might contain
         * '*' - any character sequence
         * '?' - any single character
         *
         * paging could be done with @see startWith  @see take
         * startWith=0 and take=0 means select all records;
         *
         * objects that are contained in std::vector<Dstd::shared_ptr<Directory>>& resultDirectories will be
         * 1) if this provider owned - deleted (@see ReleaseOwnership)
         * 2) if not owned - just leaved on user control
         *
         * @param  [out] resultDirectories  search result
         * @param  [in]  searchPattern      Pattern to search
         * @param  [in]  parentPath         Parent path. If NULL search through all directories
         * @param  [in]  startRecord        record number to start with
         * @param  [in]  selectRecords      number of records to select. 0 means select all records
         * @return bool true if there were error (even if 0 directories found)
         */
        virtual bool
        SearchDirectories(std::vector<std::shared_ptr<Directory>> &resultDirectories, const std::string &searchPattern,
                          const std::string &parentPath = "", int take = 0, int startWith = 0)=0;


        /** @brief SearchDirectories
         *
         * Searches directories that matches the pattern
         * inside parent directory with path @see parentPath
         * or globally through all type tables if parentPath is empty
         * The pattern might contain
         * '*' - any character sequence
         * '?' - any single character
         *
         * paging could be done with @see startWith  @see take
         * startWith=0 and take=0 means select all records;
         *
         * objects that are contained in std::vector<std::shared_ptr<Directory>>& resultDirectories will be
         * 1) if this provider owned - deleted (@see ReleaseOwnership)
         * 2) if not owned - just leaved on user control
         * @param  [in]  searchPattern      Pattern to search
         * @param  [in]  parentPath         Parent path. If NULL search through all directories
         * @param  [in]  startRecord        record number to start with
         * @param  [in]  selectRecords      number of records to select. 0 means select all records
         * @return list of
         */
        virtual std::vector<std::shared_ptr<Directory>>
        SearchDirectories(const std::string &searchPattern, const std::string &parentPath = "", int take = 0,
                          int startWith = 0);

    protected:

        /** @brief Reads all directories from DB
         *
         * Explicitly forces to load directories from DB and build directory structure
         * (!) At this implementation all existing directories references will be deleted,
         * thus  references to them will become broken
         * @return   bool
         */
        virtual bool LoadDirectories() = 0;

        virtual void
        BuildDirectoryDependencies();  /// Builds directory relational structure. Used right at the end of RetriveDirectories().
        virtual bool
        CheckDirectoryListActual();    /// Checks if directory list is actual i.e. nobody changed directories in database
        virtual bool UpdateDirectoriesIfNeeded();   /// Update directories structure if this is required


        //----------------------------------------------------------------------------------------
        //  C O N S T A N T   T Y P E   T A B L E
        //----------------------------------------------------------------------------------------

    public:
        /** @brief Gets ConstantsType information from the DB
         *
         * @param  [in] path absolute path of the type table
         * @return new object of ConstantsTypeTable
         */
        virtual ConstantsTypeTable GetConstantsTypeTable(const std::string &path, bool loadColumns);

        /** @brief Gets ConstantsType information from the DB
         *
         * @param  [in] name name of ConstantsTypeTable
         * @param  [in] parentDir directory that contains type table
         * @return new object of ConstantsTypeTable
         */
        virtual ConstantsTypeTable GetConstantsTypeTable(const std::string &name, std::shared_ptr<Directory> parentDir,
                                                         bool loadColumns = false)=0;

        /// Get all table types from DB
        /// @param loadColumns: if true - load columns from DB
        virtual std::vector<ConstantsTypeTable> GetAllTypeTables(bool loadColumns=false) = 0;

        /**
         * @brief This function counts number of type tables for a given directory
         * @param [in] directory to look tables in
         * @return number of tables to return
         */
        virtual int CountConstantsTypeTables(std::shared_ptr<Directory> dir)=0;

        /** @brief Loads columns for this table
         *
         * @param parentDir
         * @return std::vector of constants
         */
        virtual bool LoadColumns(ConstantsTypeTable *table) =0;

        //----------------------------------------------------------------------------------------
        //  V A R I A T I O N
        //----------------------------------------------------------------------------------------
        /** @brief Get variation by name
         *
         * @param     const char * name
         * @return   DVariation*
         */
        virtual std::shared_ptr<Variation> GetVariation(const std::string &name)=0;

    protected:

        virtual void EnsureVariationsLoaded() = 0;

    public:



        //----------------------------------------------------------------------------------------
        //  A S S I G N M E N T S
        //----------------------------------------------------------------------------------------
#ifndef __GNUC__
#pragma region Assignments
#endif

        /** @brief Get Assignment with data blob only
         *
         * This function is optimized for fast data retrieving and is assumed to be performance critical;
         * This function doesn't return any specified information like variation object or run-range object
         * @see GetAssignmentFull
         * @param [in] run - run number
         * @param [in] path - object path
         * @param [in] variation - variation name
         * @param [in] loadColumns - optional, do we need to load table columns information (for column names and types) or not
         * @return DAssignment object or NULL if no assignment is found or error
         */
        virtual Assignment *
        GetAssignmentShort(int run, const std::string &path, const std::string &variation = "default",
                           bool loadColumns = false)=0;


        /** @brief Get specified by creation time version of Assignment with data blob only.
         *
         * This function is optimized for fast data retrieving and is assumed to be performance critical;
         * This function doesn't return any specified information like variation object or run-range object
         * The Time is a timestamp, data that is equal or earlier in time than that timestamp is returned
         *
         * @remarks this function is named so
         * @param [in] run - run number
         * @param [in] path - object path
         * @param [in] time - timestamp, data that is equal or earlier in time than that timestamp is returned
         * @param [in] variation - variation name
         * @param [in] loadColumns - optional, do we need to load table columns information (for column names and types) or not
         * @return DAssignment object or NULL if no assignment is found or error
         */
        virtual Assignment *
        GetAssignmentShort(int run, const std::string &path, time_t time, const std::string &variation = "default",
                           bool loadColumns = false)=0;


        /** @brief Get last Assignment with all related objects
         *
         * @param     int run
         * @param     path to constant path
         * @return NULL if no assignment is found or error
         */
        virtual Assignment *
        GetAssignmentFull(int run, const std::string &path, const std::string &variation = "default")=0;


        /** @brief  Get specified version of Assignment with all related objects
         *
         * @param     int run
         * @param     const char * path
         * @param     const char * variation
         * @param     int version
         * @return   DAssignment*
         */
        virtual Assignment *
        GetAssignmentFull(int run, const std::string &path, int version, const std::string &variation = "default")=0;

        /**
         * @brief Complex and universal function to retrieve assignments
         *
         * @warning this function is too complex for users.
         *          Use overloaded GetAssignments instead of it.
         *          And only use this function if others are inapropriate
         *
         * This function is universal assignments getter.
         * Provided fields allows to select assignments (arrays and single assignments)
         * for most cases. Other GetAssignments(...) and GetAssignmentFull(...)
         * functions relie on this function.
         *
         *
         * runMin, runMax:
         *       applied if one !=0. Thus runMin=runMax=0 will select all run ranges
         *       If one needs particular run, runMin=runMax=<NEEDED RUN NUMBER> should be used
         *
         * runRangeName:
         *       will be ignored if equals to ""
         *
         * variation:
         *       if "", all variations will be get
         *
         * date:
         *       unix timestamp that indicates the latest time to select assignments from
         *       if 0 - time will be ignored
         * sortby:
         *       0 - `assignments`.`created` DESC
         *       1 - `assignments`.`created` ASC
         *
         * take, startWith
         *       paging parameters
         *
         * @param [out] assingments result assignment list
         * @param [in] path       path of type table
         * @param [in] run        specified range. If not set all ranges
         * @param [in] variation  variation, if not set, all variations
         * @param [in] date       nearest date
         * @param [in] sortBy     sortby order
         * @param [in] startWith  record to start with
         * @param [in] take       records to select
         * @return true if no errors (even if no assignments was selected)
         */
        virtual bool
        GetAssignments(std::vector<Assignment *> &assingments, const std::string &path, int runMin, int runMax,
                       const std::string &runRangeName, const std::string &variation, time_t beginTime, time_t endTime,
                       int sortBy = 0, int take = 0, int startWith = 0)=0;


        /**
         * @brief Get assigments for particular run
         *
         * returns std::vector of assignments
         * Variation: if variation is not empty string the assignments for specified variation will be returned
         *            otherwise all variations will be accepted
         *
         * Date: if date is not 0, assignments which are earlier than this date will be returned
         *       otherwise returned assignments will be not filtered by date
         *
         * Paging: paging could be done with  @see take ans @see startWith
         *         take=0, startWith=0 means select all records;
         *
         *
         * @param [out] assingments
         * @param [in]  path
         * @param [in]  run
         * @param [in]  variation
         * @param [in]  date
         * @param [in]  take
         * @param [in]  startWith
         * @return
         */
        virtual bool GetAssignments(std::vector<Assignment *> &assingments, const std::string &path, int run,
                                    const std::string &variation = "", time_t date = 0, int take = 0,
                                    int startWith = 0)=0;

        /**
         * @brief Get assigments for particular run
         *
         * returns std::vector of assignments
         * Variation: if variation is not empty string the assignments for specified variation will be returned
         *            otherwise all variations will be accepted
         *
         * Date: if date is not 0, assignments which are earlier than this date will be returned
         *       otherwise returned assignments will be not filtered by date
         *
         * Paging: paging could be done with  @see take ans @see startWith
         *         take=0, startWith=0 means select all records;
         *
         *
         * @param [in]  path
         * @param [in]  run
         * @param [in]  variation
         * @param [in]  date
         * @param [in]  take
         * @param [in]  startWith
         * @return assingments
         */
        virtual std::vector<Assignment *>
        GetAssignments(const std::string &path, int run, const std::string &variation = "", time_t date = 0,
                       int take = 0, int startWith = 0)=0;

        /**
         * @brief Get assigments for particular run
         *
         * returns std::vector of assignments
         * Variation: if variation is not emty string the assignments for specified variation will be returned
         *            otherwise all variations will be accepted
         *
         * Date: if date is not 0, assignments wich are earlier than this date will be returned
         *       otherwise returned assignments will be not filtred by date
         *
         * Paging: paging could be done with  @see take ans @see startWith
         *         take=0, startWith=0 means select all records;
         *
         *
         * @param [out] assingments
         * @param [in]  path
         * @param [in]  run
         * @param [in]  variation
         * @param [in]  date
         * @param [in]  take
         * @param [in]  startWith
         * @return
         */
        virtual bool
        GetAssignments(std::vector<Assignment *> &assingments, const std::string &path, const std::string &runName,
                       const std::string &variation = "", time_t date = 0, int take = 0, int startWith = 0)=0;

        /**
         * @brief Get assigments for particular run
         *
         * returns std::vector of assignments
         * Variation: if variation is not emty string the assignments for specified variation will be returned
         *            otherwise all variations will be accepted
         *
         * Date: if date is not 0, assignments wich are earlier than this date will be returned
         *       otherwise returned assignments will be not filtred by date
         *
         * Paging: paging could be done with  @see take ans @see startWith
         *         take=0, startWith=0 means select all records;
         *
         *
         * @param [out] assingments
         * @param [in]  path
         * @param [in]  run
         * @param [in]  variation
         * @param [in]  date
         * @param [in]  take
         * @param [in]  startWith
         * @return
         */
        virtual std::vector<Assignment *>
        GetAssignments(const std::string &path, const std::string &runName, const std::string &variation = "",
                       time_t date = 0, int take = 0, int startWith = 0)=0;


#ifndef __GNUC__
#pragma endregion Assignments
#endif

        //----------------------------------------------------------------------------------------
        //  E R R O R   H A N D L I N G
        //----------------------------------------------------------------------------------------
        /**
         * @brief Get number of errors
         * @return
         */
        virtual int GetNErrors();

        /**
         * @brief Get std::vector of last errors
         */
        virtual const std::vector<int> &GetErrorCodes();

        /** @brief return std::vector of errors.
         * @warning the error objects are deleted on next function that clears errors.
         * (!) Copy this error objects before next provider function call.
         * @return   std::vector<DCcdbError *>
         */
        virtual std::vector<CCDBError *> GetErrors();

        /**
         * @brief Gets last of the last error
         * @return error code
         */
        virtual int GetLastError();

        /** @brief Logs error
        *
        * @param errorCode Error codes see DCCDBGlobals.h
        * @param module Caller should specify method name here
        * @param message    Message of the error
        * @return   void
        */
        virtual void Error(int errorCode, const std::string &module, const std::string &message);

        /** @brief Logs error
        *
        * @param errorCode Error codes see DCCDBGlobals.h
        * @param module Caller should specify method name here
        * @param message    Message of the error
        * @return   void
        */
        virtual void Warning(int errorCode, const std::string &module, const std::string &message);

        /** @brief Clears Errors
         * function is called on start of each function that produce errors
         * @return   void
         */
        virtual void ClearErrors();

        //----------------------------------------------------------------------------------------
        //  O T H E R   F U N C T I O N S
        //----------------------------------------------------------------------------------------

        /** @brief Validates name for constant type table or directory or column
         *
         * @param     string name
         * @return   bool
         */
        bool ValidateName(const std::string &name);


    protected:

        /******* D I R E C T O R I E S   *******/
        std::vector<std::shared_ptr<Directory>> mDirectories;
        map<dbkey_t, std::shared_ptr<Directory>> mDirectoriesById;
        map<string, std::shared_ptr<Directory>> mDirectoriesByFullPath;
        bool mDirsAreLoaded;                 //Directories are loaded from database
        bool mVariationsAreLoaded;
        bool mNeedCheckDirectoriesUpdate;    //Do we need to check each time iff directories are updated or not
        std::shared_ptr<Directory> mRootDir;                 ///root directory. This directory contains all other directories. It is not stored in databases


        /**
            @brief Clear error state on start of each function that emits error
         */
        virtual void ClearErrorsOnFunctionStart();

        std::vector<int> mErrorCodes;            ///vector of last errors

        std::vector<CCDBError *> mErrors;        ///errors

        int mLastError;                     ///last error

        const int mMaximumErrorsToHold;     ///=100 Maximum errors to hold in @see mLastErrors

        std::string mConnectionString;      ///Connection string that was used on last successfully connect.

        map<dbkey_t, std::shared_ptr<Variation>> mVariationsById;

    private:
        std::string mClassName;                 /// Used for error reporting
    };
}
#endif // _DDataProvider_

