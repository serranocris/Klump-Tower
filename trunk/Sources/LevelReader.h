// Joseph Onesto, Alex Jonic
// Early level reader header
// Reads in a level from a text file

#ifndef __LEVELREADER_H
#define __LEVELREADER_H

#include "Frog.h"

namespace Webfoot
{
    class LevelReader;
    
    class LevelReader
    {
    
        public:
          LevelReader();
          void Init();
          void Deinit();
          // No need to update or draw
          
          // No need for any data members
          
          void ReadLevel(std::string filePath);
	};

}

#endif