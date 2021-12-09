
#include <string>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
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

		// printSame print index and same file
		void printSame( void )
		{
			for (std::map<size_t, std::vector<std::string> >::iterator it = _result.begin(); it != _result.end(); ++it)
			{
				std::cout << it->first;
				for (size_t i = 0; i < it->second.size(); ++i)
				{
					std::cout << " " << it->second[i];
				}
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
			if (!(boost::filesystem::is_directory(_dirNameFirst) && boost::filesystem::is_directory(_dirNameSecond)))
				throw ("directory not found");
        }

		std::vector<std::string> __readFileInDir(const std::string & directory)
		{

			std::vector<std::string> result;
			for (boost::filesystem::recursive_directory_iterator it(directory), end; it != end; ++it)
				if (boost::filesystem::is_regular_file(*it))
					result.push_back(it->path().generic_string());


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

		void __findHashAllfile(std::vector<std::string> listFile)
		{
			for (size_t i = 0; i < listFile.size(); ++i)
			{
				int fd = open((listFile[i]).c_str(), O_RDONLY);
				if (fd == -1)
					continue;
				unsigned int hval = HVAL_START;
				while (1)
				{
					int i = read(fd, &buff, BUFFER_SIZE - 1);
					buff[i] = 0;
					hval = __hashFile(buff, hval);
					if (i != BUFFER_SIZE - 1)
						break;
				}
				_same[hval].push_back(listFile[i]);
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
			__findHashAllfile(__readFileInDir(_dirNameFirst));
			__findHashAllfile(__readFileInDir(_dirNameSecond));
			__makeMapIndex();
		}
};