#ifndef SAMEBINARY_H
#define SAMEBINARY_H

#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
// this class takes two absolute path and find all same file(binary)
// You can get this using the getAllSame function
const unsigned FNV_32_PRIME = 0x01000193;
const unsigned HVAL_START = 0x811c9dc5;
#define BUFFER_SIZE 100000

class SameBinary
{

    public:
        SameBinary(const std::string & firstDir, const std::string & secondDir) : _dirNameFirst(firstDir), _dirNameSecond(secondDir)
        {
            if (firstDir == secondDir)
                throw "The same directory!";
            __initMap();
        }

        ~SameBinary()
        {}

        //change directory
        void setNewDir(const std::string & firstDir, const std::string & secondDir)
        {
            _same.clear();
            _dirNameFirst = firstDir;
            _dirNameSecond = secondDir;
            __initMap();
        }

        // getAllSame return map<size_t index, vector<std::string array_files_name> >
        std::map<size_t, std::vector<std::string> > getAllSame( void )
        {
            return _result;
        }

        std::string getAllSameInStr()
        {
            std::string result;

            for (std::map<size_t, std::vector<std::string> >::iterator it = _result.begin(); it != _result.end(); ++it)
            {
                result += std::to_string(it->first);
                for (size_t i = 0; i < it->second.size(); ++i)
                    result += ' ' + it->second[i];
                result += '\n';
            }
            return result;
        }

        // printSame print index and same file
        void printSame( void )
        {
            for (std::map<size_t, std::vector<std::string> >::iterator it = _result.begin(); it != _result.end(); ++it)
            {
                std::cout << it->first;
                for (size_t i = 0; i < it->second.size(); ++i)
                    std::cout << " " << it->second[i];
                std::cout << std::endl;
            }
        }

    private:
        std::string 								_dirNameFirst;
        std::string 								_dirNameSecond;
        std::map<size_t, std::vector<std::string> > _same;
        std::map<size_t, std::vector<std::string> > _result;
        char 										buff[BUFFER_SIZE];

        SameBinary(const SameBinary & copy);
        SameBinary & operator=(const SameBinary);
        SameBinary();

        void __checkDirExist( void )
        {
            if (!(std::filesystem::is_directory(_dirNameFirst) && std::filesystem::is_directory(_dirNameSecond)))
                throw ("Directory not found!");
        }

        std::map<unsigned long, std::vector<std::string> > __readFileInDir(const std::string & directoryFirst, const std::string & directorySecond)
        {

            std::map<unsigned long, std::vector<std::string> > result;

            for (std::filesystem::recursive_directory_iterator it(directoryFirst), end; it != end; ++it)
                if (std::filesystem::is_regular_file(*it))
                    result[std::filesystem::file_size(it->path())].push_back((it->path().generic_string()));

            for (std::filesystem::recursive_directory_iterator it(directorySecond), end; it != end; ++it)
                if (std::filesystem::is_regular_file(*it))
                    result[std::filesystem::file_size(it->path())].push_back((it->path().generic_string()));

            for (std::map<unsigned long, std::vector<std::string> >::iterator it = result.begin(); it != result.end(); ++it)
            {
                if (it->second.size() <= 1)
                {
                    result.erase(it);
                    if (result.size() == 0)
                        break;
                    it = result.begin();
                    continue;
                }
            }

            return result;
        }

        unsigned int __hashFile(char * buff, unsigned int hval)
        {
            while (*buff)
            {
                hval ^= (unsigned int)*buff++;
                hval *= FNV_32_PRIME;
            }
            return hval;
        }

        struct fileParam
        {
            unsigned int	hash;
            std::string		fileName;
            unsigned int	fd;
        };

        void __findSameAllfile(std::vector<std::string> listFile, unsigned long sizeFiles)
        {
            std::vector<std::string> result;
            std::vector<fileParam>	 file;

            for (size_t i = 0; i < listFile.size(); ++i)
            {
                int fd = _open((listFile[i]).c_str(), _O_RDONLY);
                if (fd == -1)
                {
                    listFile.erase(listFile.begin() + i);
                    i = 0;
                    continue;
                }
                fileParam tmp;
                tmp.hash = HVAL_START;
                tmp.fileName = listFile[i];
                tmp.fd = fd;
                file.push_back(tmp);
            }
            if (file.size() == 0)
                return ;
            int size_read = 0;
            int readSize;
            if (sizeFiles < 1000)
                readSize = 100;
            else if (sizeFiles < 10000)
                readSize = 1000;
            else if (sizeFiles < 100000)
                readSize = 10000;
            else
                readSize = 100000;
            while (1)
            {
                std::unordered_map<unsigned int, size_t> findOneFile;
                for (size_t i = 0; i < file.size(); ++i)
                {
                    size_read = _read(file[i].fd, &buff, readSize - 1);
                    buff[size_read] = 0;
                    file[i].hash = __hashFile(buff, file[i].hash);
                    findOneFile[file[i].hash] += 1;
                }
                for (std::unordered_map<unsigned int, size_t>::iterator it = findOneFile.begin(); it != findOneFile.end(); ++it)
                {
                    if (it->second <= 1)
                    {
                        for (size_t i = 0; i < file.size(); ++i)
                        {
                            if (file[i].hash == it->first)
                            {
                                file.erase(i + file.begin());
                                break;
                            }
                        }
                    }
                }

                if (size_read == 0 || file.size() <= 0)
                    break;
            }
            for (size_t i = 0; i < file.size(); ++i)
            {
                _close(file[i].fd);
                _same[file[i].hash].push_back(file[i].fileName);
            }
        }

        void __makeMapIndex( void )
        {
            size_t index = 1;
            for (std::map<size_t, std::vector<std::string> >::iterator it = _same.begin(); it != _same.end(); ++it)
            {
                if (it->second.size() <= 1)
                    continue;
                _result[index] = it->second;
                ++index;
            }
        }

        void __initMap( void )
        {
            __checkDirExist();
            std::map<unsigned long, std::vector<std::string> > tmpMap = __readFileInDir(_dirNameFirst, _dirNameSecond);
            for (std::map<unsigned long, std::vector<std::string> >::iterator it = tmpMap.begin(); it != tmpMap.end(); ++it)
                __findSameAllfile(it->second, it->first);
            if (_same.size() == 0)
                throw ("Same file not found!");
            __makeMapIndex();
        }
};

#endif // SAMEBINARY_H
