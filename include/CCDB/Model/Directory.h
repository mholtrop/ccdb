/*
 * Directory.h
 *
 *  Created on: Sep 15, 2010
 *      Author: romanov
 */

#ifndef CONSTANTDIRECTORY_H_
#define CONSTANTDIRECTORY_H_

#include <vector>
#include <string>
#include <ctime>

#include "CCDB/Globals.h"


namespace ccdb{

class Directory
{

public:

    Directory();    /// Def ctor


    /**
     * @brief Get
     * @return pointer to parent directory. NULL if there is no parent directory
     */
    std::shared_ptr<Directory> GetParentDirectory();

    /**
     * @brief Gets the vector of pointers to subdirectories
     * @return vector of pointers to subdirectories
     */
    const std::vector<std::shared_ptr<Directory>> GetSubdirectories();

    /**
     * @brief Adds a subdirectory of this directory
     *
     * Adds a subdirectory of this directory
     * Automatically adds "this" as mParent for child
     *
     * @param subDirectory Child directory to be added
     */
    void AddSubdirectory(std::shared_ptr<Directory> subdirectory);


    dbkey_t	GetId() const;				///DB id
    void	SetId(dbkey_t val);			///DB id

    dbkey_t	GetParentId() const;		///DB id of parent directory. Id=0 - root directory
    void	SetParentId(dbkey_t val);	///DB id of parent directory. Id=0 - root directory

    string	GetName() const;			///Name of the directory
    void	SetName(const std::string&val);		///Name of the directory

    string	GetFullPath() const;		///Full path (including self name) of the directory
    void	SetFullPath(const std::string&val);	///Full path (including self name) of the directory

    time_t	GetCreatedTime() const;		///Creation time
    void	SetCreatedTime(time_t val);	///Creation time

    time_t	GetModifiedTime() const;	///Last modification time
    void	SetModifiedTime(time_t val);///Last modification time

    string	GetComment() const;			///Full description of the directory
    void	SetComment(const std::string&val);		///Full description of the directory
protected:



private:
    string mName;		///Name of directorey like in db
    string mFullPath;	///full path
    string mComment;	///Comment like in db
    std::shared_ptr<Directory> mParent;
    std::vector<std::shared_ptr<Directory>> mSubDirectories;
    dbkey_t mParentId;
    dbkey_t mId;
    time_t mCreatedTime;
    time_t mModifiedTime;

};

}
#endif /* CONSTANTDIRECTORY_H_ */
