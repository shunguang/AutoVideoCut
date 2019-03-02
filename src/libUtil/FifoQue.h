/*
*-------------------------------------------------------------------
* FifoQue.h
* Copyright(c) 2019, The RgbVideoEditor Author (Shunguang Wu).
* All Rights Reserved.
* You may not use this file except in compliance with the License:
*      GNU GENERAL PUBLIC LICENSE (Version 3), 29 June 2007
*
* You may obtain a copy of the License at
*
*     https://www.gnu.org/licenses/gpl-3.0.en.html
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*-------------------------------------------------------------------
*/
#ifndef _FIFO_QUE_H_
#define _FIFO_QUE_H_

#include "DataTypes.h"
#include "UtilDefs.h"
namespace autocut {
template<typename Data>
class UTIL_EXPORT FifoQue
{
private:
int qsize;
std::queue<Data> the_queue;
mutable boost::mutex the_mutex;
boost::condition_variable the_condition_variable;
public:
FifoQue(const int qSize_ = 10) : qsize(qSize_) 
{}

bool isFull() {
int s = size();
if (s >= qsize) {
return true;
}
else {
return false;
}
}

int size() {
int s = 0;
{
boost::mutex::scoped_lock lock(the_mutex);
s = the_queue.size();
}
return s;
}

bool empty() const
{
boost::mutex::scoped_lock lock(the_mutex);
return the_queue.empty();
}

void clear()
{
boost::mutex::scoped_lock lock(the_mutex);
while(!the_queue.empty()){
the_queue.pop();
}
}

void push(Data const& data)
{
boost::mutex::scoped_lock lock(the_mutex);
the_queue.push(data);
lock.unlock();
the_condition_variable.notify_one();
}

bool try_pop(Data& popped_value)
{
boost::mutex::scoped_lock lock(the_mutex);
if (the_queue.empty())
{
return false;
}

popped_value = the_queue.front();
the_queue.pop();
return true;
}

void wait_and_pop(Data& popped_value)
{
boost::mutex::scoped_lock lock(the_mutex);
while (the_queue.empty())
{
the_condition_variable.wait(lock);
}

popped_value = the_queue.front();
the_queue.pop();
}
};
}
#endif
