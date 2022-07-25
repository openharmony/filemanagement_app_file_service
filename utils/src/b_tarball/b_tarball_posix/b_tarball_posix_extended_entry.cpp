/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_extended_entry.h"

#include <cstdio>
#include <linux/limits.h>
#include <sys/stat.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

optional<BTarballPosixExtendedEntry> BTarballPosixExtendedEntry::TryToGetEntry(BConstants::EntryKey entryKey,
                                                                               const string &pathName,
                                                                               const struct stat &statInfo)
{
    switch (entryKey) {
        case BConstants::SUPER_LONG_PATH:
            if (pathName.size() > BConstants::PATHNAME_MAX_SIZE - 1) {
                return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_PATH, pathName);
            }
            break;
        case BConstants::SUPER_LONG_LINK_PATH:
            switch (statInfo.st_mode & S_IFMT) {
                case S_IFLNK: {
                    char linkName[PATH_MAX] {};
                    ssize_t linkSize =
                        readlink(pathName.c_str(), linkName, PATH_MAX); // return size of link name without nul
                    if (linkSize > BConstants::LINKNAME_MAX_SIZE - 1) {
                        return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_LINKPATH, linkName);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        case BConstants::SUPER_LONG_SIZE:
            if (statInfo.st_size > BConstants::FILESIZE_MAX) {
                return BTarballPosixExtendedEntry(BConstants::ENTRY_NAME_SIZE, to_string(statInfo.st_size));
            }
            break;
    }
    return {};
}

size_t BTarballPosixExtendedEntry::GetEntrySize()
{
    return entrySize_;
}

string BTarballPosixExtendedEntry::GetEntry()
{
    return entry_;
}

void BTarballPosixExtendedEntry::Publish(const UniqueFd &outFile)
{
    if (write(outFile, entry_.c_str(), entry_.size()) == -1) {
        printf("BTarballPosixExtendedEntry::Publish: write\n");
        throw exception();
    }
}

BTarballPosixExtendedEntry::BTarballPosixExtendedEntry(const string &entryName, const string &entryValue)
    : entryName_(entryName), entryValue_(entryValue)
{
    size_t fieldSize = entryName_.size() + entryValue_.size() + string_view(" =\n").size();
    size_t digitsNum = 0;
    size_t tmp = fieldSize;
    size_t nextHigherPowerOfTen = 1;
    while (tmp > 0) {
        tmp /= BConstants::DECIMAL_BASE;
        ++digitsNum;
        nextHigherPowerOfTen *= BConstants::DECIMAL_BASE;
    }
    if (digitsNum + fieldSize >= nextHigherPowerOfTen) {
        ++digitsNum;
    }
    entrySize_ = digitsNum + fieldSize;
    entry_ = to_string(entrySize_) + " " + entryName_ + "=" + entryValue_ + "\n";
}
} // namespace OHOS::FileManagement::Backup