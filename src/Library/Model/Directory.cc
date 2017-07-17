/*
 * Directory.cpp
 *
 *  Created on: Sep 28, 2010
 *      Author: romanov
 */

#include <bits/shared_ptr.h>
#include "CCDB/Model/Directory.h"

using namespace std;
namespace ccdb
{


    Directory::Directory() :
            mSubDirectories(0),
            mName(""),
            mFullPath(""),
            mComment(""),
            mParent(nullptr),
            mId(0),
            mParentId(0)
    {
    }


    void ccdb::Directory::AddSubdirectory(std::shared_ptr<Directory> subdirectory)
    {
        subdirectory->mParent = this;
        mSubDirectories.push_back(subdirectory);
    }

    std::shared_ptr<Directory> ccdb::Directory::GetParentDirectory()
    { return mParent; }

    const vector<shared_ptr<Directory>> ccdb::Directory::GetSubdirectories()
    { return mSubDirectories; }


    string ccdb::Directory::GetName() const
    { return mName; }

    void ccdb::Directory::SetName(const std::string&val)
    { mName = val; }


    std::string ccdb::Directory::GetFullPath() const
    { return mFullPath; }

    void ccdb::Directory::SetFullPath(const std::string&val)
    { mFullPath = val; }


    std::string ccdb::Directory::GetComment() const
    { return mComment; }

    void ccdb::Directory::SetComment(const std::string&val)
    { mComment = val; }


    dbkey_t ccdb::Directory::GetParentId() const
    { return mParentId; }
->
    void ccdb::Directory::SetParentId(dbkey_t val)
    { mParentId = val; }


    int ccdb::Directory::GetId() const
    { return mId; }

    void ccdb::Directory::SetId(dbkey_t val)
    { mId = val; }


    time_t ccdb::Directory::GetCreatedTime() const
    { return mCreatedTime; }

    void ccdb::Directory::SetCreatedTime(time_t val)
    { mCreatedTime = val; }


    time_t ccdb::Directory::GetModifiedTime() const
    { return mModifiedTime; }

    void ccdb::Directory::SetModifiedTime(time_t val)
    { mModifiedTime = val; }
}









