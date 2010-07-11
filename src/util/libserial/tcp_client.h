// copyright 2009 t. schneider tes@mit.edu
// 
// this file is part of comms, a library for handling various communications.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TCPClientH
#define TCPClientH

#include "client_base.h"

namespace tcp
{    
    class TCPClient : public comms::ClientBase<asio::ip::tcp::socket>
    {
      public:
        static TCPClient* getInstance(const std::string& server,
                                      const std::string& port,
                                      std::deque<std::string>* in,
                                      boost::mutex* in_mutex);

      private:

        TCPClient(const std::string& server,
                  const std::string& port,
                  std::deque<std::string>* in,
                  boost::mutex* in_mutex,
                  std::string delimiter = "\r\n");

        bool start_specific();

        
      private:
        static std::map<std::string, TCPClient*> inst_;
        asio::ip::tcp::socket socket_;
        std::string server_;
        std::string port_;

    }; 
}

#endif