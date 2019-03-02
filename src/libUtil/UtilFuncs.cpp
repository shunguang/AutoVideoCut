/*
*-------------------------------------------------------------------
* UtilFuncs.cpp
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
#include "UtilFuncs.h"

using namespace std;
using namespace boost;

string autocut::num_to_string( const bool x, const std::string &msg )
{
string s("true");
if ( !x ){
s = "false";
}

return msg+s;
}

std::string  autocut::num_to_string( const uint8 x,	const std::string &msg )
{
ostringstream convert;   // stream used for the conversion
convert << (int)x;		     // insert the textual representation of 'x' in the characters in the stream
return msg+convert.str();	
}

std::string  autocut:: num_to_string( const uint16 x,	const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}

string autocut::num_to_string( const uint32 x, const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}

string autocut::num_to_string( const uint64 x, const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}

string autocut::num_to_string( const int32 x, const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}



string autocut::num_to_string( const float x, const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}

string autocut::num_to_string( const double x, const std::string &msg )
{
ostringstream convert;
convert << x;
return msg+convert.str();	
}

std::string autocut::vec_to_string( const std::vector<std::string> &v, const std::string &msg )
{
ostringstream convert;   // stream used for the conversion
convert << msg << endl;		     
convert << "Total Items" << v.size() << endl;
vector<string>::const_iterator it=v.begin();
for (; it!=v.end(); ++it ){
convert << *it << endl;		     
}
return msg+convert.str();	
}

bool autocut::string_in_vector( const std::string &s, const std::vector<std::string> &v  )
{
vector<string>::const_iterator it=v.begin();
for (; it!=v.end(); ++it ){
if ( 0 == s.compare( *it ) ){
return true;
}
}
return false;
}

int autocut::string_in_vector_relax( const std::string &s, const std::vector<std::string> &v  )
{
vector<string>::const_iterator it=v.begin();
int i=0;
for (; it!=v.end(); ++it, ++i ){
if ( string::npos != (*it).find( s ) ){
return i;
}
}
return -1;
}

bool autocut::string_to_bool( const std::string &in )
{
string s(in);
for ( uint32 i=0; i<s.size(); ++i ){
s[i] = std::toupper( s[i] );
}

if ( 0==s.compare("YES") || 0==s.compare("Y") ){
return true;
}
else if ( 0==s.compare("TRUE") || 0==s.compare("T") ){
return true;
}
else if( 0==s.compare("1") ){
return true;
}
else{
return false;
}
}

uint64	autocut::string_to_uint64( const std::string &s )
{
uint64 x;
stringstream strm(s);

strm >> x;

return x;
}

uint32 autocut::string_to_vector( vector<string> &v, const string &s, const string &key, const bool keepKey )
{
boost::regex re( key );
sregex_token_iterator it( s.begin(), s.end(), re, -1);
boost::sregex_token_iterator j;

v.clear();

if( keepKey ){
while( it != j ){
//cout << key+*it << endl;
v.push_back( key+*it++ );
}
}
else{
while( it != j ){
//cout << *it << endl;
v.push_back( *it++ );
}
}

return v.size();
}

uint32 autocut::string_to_vector2( vector<string> &v, const string &s, const string &key, const bool keepKey )
{

v.clear();

uint32 L = key.size();
uint32 pos = 0;
std::string::size_type found = s.find(key, pos);
while( found !=std::string::npos ){
uint32 len = found - pos;
if( keepKey ){
len += L;
}
if( len>0 ){
v.push_back( s.substr( pos, len) );
}

pos = found + L;
found = s.find(key, pos);
}
if( pos <= s.size()-1 ){
v.push_back( s.substr( pos ) );
}

return v.size();
}

std::string	autocut :: vector_to_string( const std::vector<std::string> &v, const std::string &seperateKey )
{
if ( v.size()==0 ){
return "";
}

string s=v[0];
for( uint32 i=1; i<v.size(); ++i ){
s += (seperateKey + v[i]);
}

return s;
}


void autocut :: string_trim( string &x, const char ch )
{
if ( x.empty() ){
return;
}

//ersase end
string::iterator it = x.end();
while( *(--it) == ch ){
x.erase(it);
if( x.empty() ){
return;
}
it = x.end();
}

//erase begin
it = x.begin();
while( *it==ch ){
x.erase( it );
if( x.empty() ){
return;
}
it = x.begin();
}

}


//---------------------------------
/*
s = "[$SYSTEMNAME]> show statworpsutbl
WORP SU Statistics Table
------------------------
Channel Bandwidth               :                 10 MHz
SU MAC Address                  :               00:20:A6:C1:5F:C8
SU Local Tx Rate                :                 6 Mbps
SU Remote Tx Rate               :                 6 Mbps
SU Average Local Signal         :                    -33
SU Average Local Noise          :                    -89
SU Average Remote Signal        :                    -35
SU Average Remote Noise         :                    -92
SU Distance                     :               100 meters"
*/
//------------------------------------------------------------------
// call string_parse(s, "Channel Bandwidth" ) return "10 MHz"

std::string autocut::string_parse( const std::string &x, const std::string &key )
{
string y;

uint32 found = x.find( key );
if ( found == string::npos ){
return "";
}

y = x.substr( found+key.size() );

found = y.find( ':' );
y = y.substr( found+1 );
found = y.find( '\r' );
y = y.substr(0, found );

string_trim(y, ' ');
string_trim(y, '\t');
string_trim(y, '\n');
string_trim(y, ' ');

return y;
}
//        01234567890123456    
// mac = "01-23-45-67-89-ab" or 01:23:45:67:89:ab
std::string autocut::string_find_mac( const std::string &x, const char &seperationKey )
{
int L = (int)(x.size());
for( int i=0; i<=L-17; ++i ){
if( x[i+2]==seperationKey && x[i+5]==seperationKey && x[i+8]==seperationKey && x[i+11]==seperationKey && x[i+14]==seperationKey ){
return x.substr(i, 17 );
}
}
return "";
}

//remove the beg and end chars if any of them are in $keys$
void autocut:: string_trim( string &x, const std::vector<char> &keys )
{
if ( x.empty() ){
return;
}
uint32 n=keys.size();

//ersase end
string::iterator it = x.end();
while( 1 ){
char ch = *(--it);
bool found=false;
for (uint32 i=0; i<n; ++i){
if ( ch==keys[i] ){
found=true;
break;
}
}

if( !found ){
break;
}
else{
x.erase(it);
if( x.empty() ){
return;
}
it = x.end();
}
}

//erase begin
it = x.begin();
while( 1 ){
char ch = *it;
bool found=false;
for (uint32 i=0; i<n; ++i){
if ( ch==keys[i] ){
found=true;
break;
}
}

if( !found ){
break;
}
else{
x.erase( it );
if( x.empty() ){
return;
}
it = x.begin();
}
}

}

void	 autocut:: string_find_n_replace( std::string &x, const char chFind, const char chRaplace )
{
for(uint32 i=0; i<x.size(); ++i){
if ( x[i]==chFind ) {
x[i] = chRaplace;
}
}
}


void autocut::vstr_read_txt_file( std::vector<std::string> &vLines, const std::string &fileName, const char commentChar )
{

std::string line;
std::ifstream myfile( fileName );

while (std::getline(myfile, line)) {
autocut::string_trim( line, ' ' );

if( line.empty() ){
continue;
}
if ( line[0]==commentChar ){
continue;
}

vLines.push_back(line);	
}
}

void autocut:: vstr_select_util( std::vector<std::string> &retLines, const std::vector<std::string> &vAllLines, const std::string &startKey, const std::string &stopKey )
{
retLines.clear();
uint32 n =vAllLines.size();
for ( uint32 i=0; i<n; ++i ){
const string &line = vAllLines[i];
if( string::npos == line.find( startKey ) ){
continue;
}

uint32 j=i+1;
while(j<n){
const string &nextLine = vAllLines[j];
if( string::npos == nextLine.find(stopKey) ){
retLines.push_back( nextLine );
}
else{
return;
}
++j;
}
}
}

//---------------------------------------------------------------
//given vector of lines such as:
//
//ipv4=10.0.4.101
//macWired=ba:wi:cc:dd:ee:01
//macWireless=ba:wl:cc:dd:ee:01
//
//query value (RHS) from key (LHS)
//
//example: vstr_find_value("ipv4") will return "10.0.4.101"
//---------------------------------------------------------------

std::string autocut::vstr_find_value( const std::vector<std::string> &vLines, const std::string &key )
{
std::vector<std::string>::const_iterator it=vLines.begin();
std::vector<std::string> kv;
for ( ; it!=vLines.end(); ++it ){
autocut::string_to_vector( kv, *it, "=", false );
if ( kv.size()<2 ){
continue;
}
if ( 0 == key.compare( kv[0] ) ){
return kv[1];
}
}

return "";
}

std::string autocut::getPrettyTimeStamp(const bool dayTimeOnly)
{
boost::posix_time::ptime now = POSIX_LOCAL_TIME;  
return getPrettyTimeStamp(now, dayTimeOnly);
}

std::string autocut::getPrettyTimeStamp(const boost::posix_time::ptime &t, const bool dayTimeOnly)
{
string stimeStamp = to_simple_string(t);  //[YYYY-mmm-DD HH:MM:SS.fffffffff]
if (dayTimeOnly){
return stimeStamp.substr(12, 11);
}

autocut::string_find_n_replace(stimeStamp, ' ', '-');
autocut::string_find_n_replace(stimeStamp, ':', '-');
autocut::string_find_n_replace(stimeStamp, '.', '-');
return stimeStamp;
}

std::string autocut::timeSecToHHMMSSxx(const float seconds)
{
float t0 = seconds;
if ( t0 > 86400 ) {
t0 = (uint32)t0 % 86400;
}

int h = 0, m = 0, s = 00, x = 0;

h = floor(t0 / 3600);
t0 -= (h * 3600);
if (t0 > 0) {
m = floor(t0 / 60);
t0 -= (m * 60);
if (t0 > 0) {
s = floor(t0);             //sec
t0 -= s;
//assert(t0 < 1);
x = floor(1000*t0);        //ms
}
}
char buf[64];
snprintf(buf, 64, "%02d:%02d:%02d.%03d", h, m, s, x);
return std::string(buf);
}

uint32  autocut::checkTimeOut( const boost::posix_time::ptime &start, const uint32 thdInMillisec )
{
boost::posix_time::ptime now = POSIX_LOCAL_TIME;
boost::posix_time::time_duration dt = now - start;

int64 dt_milliseconds = dt.total_milliseconds();

if( dt_milliseconds >= thdInMillisec ){
//already timeout
return 0;
}
else{
return thdInMillisec - dt_milliseconds;
}
}

uint32  autocut::timeIntervalMillisec( const boost::posix_time::ptime &start )
{
boost::posix_time::ptime end = POSIX_LOCAL_TIME;
boost::posix_time::time_duration dt = end - start;

return (uint32)(dt.total_milliseconds());
}

uint32 autocut::timeIntervalMillisec( const boost::posix_time::ptime &start, const boost::posix_time::ptime &end )
{
boost::posix_time::time_duration dt = end - start;
return (uint32)(dt.total_milliseconds());
}


int8 autocut::int32_to_int8( const int32 x )
{
if ( x < -128 )
return -128;
else if( x>127 )
return 127;
else 
return x;
}


void autocut::printfStr(const std::string &s, const std::string &msg)
{
uint32 n = s.size();
printf( "%s, (n=%d): ", msg.c_str(),n );
for (uint32 i = 0; i < n; ++i){
printf("(%d,%c,%d);", i, s[i], s[i]);
}
printf("\n");
}

void autocut::myCreateDir(const std::string &p)
{
boost::filesystem::path p0(p);
if (!boost::filesystem::exists(p0)) {
if (!boost::filesystem::create_directories(p0)) {
printf("autocut::myCreateDir(): cannot create root folder:%s", p0.string().c_str());
}
}
}

//generate about <n> distinct/unique uniform distributed random number in [a,b]
void autocut::genUniformDistinctRands(std::vector<int> &v, const int n, const int a, const int b)
{
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator(seed);
std::uniform_int_distribution<int> distribution(a, b);

v.resize(n);
for (int i = 0; i < n; ++i) {
v[i] = distribution(generator);
}

//v ={1,2,3,1,2,3,3,4,5,4,5,6,7}
std::sort(v.begin(), v.end());  // 1 1 2 2 3 3 3 4 4 5 5 6 7 
auto last = std::unique(v.begin(), v.end());
// v now holds {1 2 3 4 5 6 7 x x x x x x}, where 'x' is indeterminate
v.erase(last, v.end());
}

void autocut::rgbFromStr(uint8 *rgb, const std::string &s)
{
vector<string> v;
string_to_vector2(v, s, ",", false);

rgb[0] = atoi(v[0].c_str());
rgb[1] = atoi(v[1].c_str());
rgb[2] = atoi(v[2].c_str());
}

std::string autocut::rgbToStr(const uint8 *rgb)
{
std::string str = std::to_string(rgb[0]) + "," + std::to_string(rgb[1]) + "," + std::to_string(rgb[2]);

return str;
}


void autocut::split(const std::string &fPath, std::string &head, std::string &tail)
{
int id = fPath.find_last_of('/');
if (id == std::string::npos) return;

head = fPath.substr(0, id);
tail = fPath.substr(id+1);
}

void autocut::splitExt(const std::string &fPath, std::string &head, std::string &ext)
{
int id = fPath.find_last_of('.');
if (id == std::string::npos) return;

head = fPath.substr(0, id);
ext = fPath.substr(id + 1);
}

uint32_t autocut::getFileNameList(const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames)
{

_finddata_t c_file;
intptr_t hFile;

_chdir(dirName.c_str());

// Find any file in current directory
if ((hFile = _findfirst("*.*", &c_file)) == -1) {
printf("CaptureFrmImg :: getFileNameList(): Error!\n"); // This doesn't mean current directory is empty.
return 0;
}

//read file names into $vRetNames$
string ext0 = ext;
boost::to_upper(ext0);
vFileNames.clear();
do {
// if the file is not a sub dir...
string fName, base, extName;
if ((c_file.attrib & _A_SUBDIR) != _A_SUBDIR) { // _A_SUBDIR = 0x10 (16 in decimal). Also note the single &.
//printf( "%s\n", c_file.name );
fName = string(c_file.name);
splitExt(fName, base, extName);
boost::to_upper(extName);
if (0 == ext0.compare(extName) || 0 == ext.compare("*")) {
vFileNames.push_back(fName);
}
}
} while (_findnext(hFile, &c_file) == 0); // find next file
_findclose(hFile);

//sort file names
sort(vFileNames.begin(), vFileNames.end());

#if 0
for (size_t i = 0; i < vFileNames.size(); ++i) {
printf("i=%d, %s\n", i, vFileNames[i].c_str());
}
#endif

return vFileNames.size();
}
