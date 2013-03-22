#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <vector>
#include <utility>

#include "client/dbclient.h"

namespace Tools
{
	template<typename Key, typename Value>
	std::map<Value, Key> swapMap(const std::map<Key, Value>& map)
	{
		std::map<Value, Key> res;
		for(auto ci = map.begin(); ci != map.end(); ci++)
			res[ci->second] = ci->first;
		return res;
	}

	template<typename T> inline T get(mongo::BSONElement e);
	template<> inline int get<int>(mongo::BSONElement e){ return e.numberInt(); }
	template<> inline double get<double>(mongo::BSONElement e){ return e.numberDouble(); }
	template<> inline std::string get<std::string>(mongo::BSONElement e){ return e.str(); }

	template<typename T> inline T get(const char*);
	template<> inline int get<int>(const char* s){ return std::atoi(s); }
	template<> inline double get<double>(const char* s){ return std::atof(s); }
	template<> inline std::string get<std::string>(const char* s){ return s; }

	template<class Array>
	void copy(const std::vector<mongo::BSONElement>& source, Array& dest)
	{
		for(size_t i = 0, ssize = source.size(), dsize = dest.size(); i < ssize && i < dsize; i++)
			dest[i] = get<typename Array::value_type>(source[i]);
	}

	template<class Array>
	void copy(const mongo::BSONElement& fromArray, Array& dest)
	{
		copy(fromArray.Array(), dest);
	}

//	template<class Map>
//	void fillMap(Map& m, const std::vector<mongo::BSONElement>& es,
//							 std::string keyName, std::string valueName, bool clearMap = false)
//	{
//		if(clearMap)
//			m.clear();

//		for(auto it = es.begin(); it != es.end(); it++)
//			m[get<typename Map::key_type>((*it)[keyName])] =
//					get<typename Map::mapped_type>((*it)[valueName]);
//	}

//	template<class Map>
//	void fillMap(Map& m, mongo::BSONElement& e, std::string keyName,
//							 std::string valueName, bool clearMap = false)
//	{
//		fillMap(m, e.Array(), keyName, valueName, clearMap);
//	}

	template<class Map>
	void fillMap(Map& m, mongo::BSONObj& o, bool clearMap = false)
	{
		if(clearMap)
			m.clear();

		for(BSONObj::iterator i = o.begin(); i.more(); )
		{
			BSONElement e = i.next();
			m[get<typename Map::key_type>(e.fieldName())] =
					get<typename Map::mapped_type>(e);
		}
	}

//	template<typename KeyType, typename ValueType>
//	std::map<KeyType, ValueType> createMap(const std::vector<mongo::BSONElement>& es,
//																		std::string keyName, std::string valueName)
//	{
//		std::map<KeyType, ValueType> m;
//		fillMap(m, es, keyName, valueName, false);
//		return m;
//	}

//	template<typename KeyType, typename ValueType>
//	std::map<KeyType, ValueType> createMap(mongo::BSONElement& e,
//																				 std::string keyName, std::string valueName)
//	{
//		return createMap(e.Array(), keyName, valueName);
//	}

	template<typename Value>
	std::pair<Value, bool> success(Value val)
	{
		return std::make_pair(val, true);
	}

	template<typename Value>
	bool wasSuccess(const std::pair<Value, bool>& p){ return p.second; }

	template<typename Value>
	std::pair<Value, bool> error(){ return std::make_pair(Value(), false); }

	template<typename Value>
	Value getValue(const std::pair<Value, bool>& p){ return p.first; }

	template<class Array>
	mongo::BSONArray toBSONArray(const Array& array)
	{
		mongo::BSONArrayBuilder b;
		for(auto ci = array.begin(); ci != array.end(); ci++)
			b << *ci;
		return b.arr();
	}

	template<class Map>
	mongo::BSONObj toBSONObject(const Map& map)
	{
		mongo::BSONObjBuilder b;
		for(auto ci = map.begin(); ci != map.end(); ci++)
		{
			std::ostringstream ss;
			ss << ci->first;
			b << ss.str() << ci->second;
		}
		return b.obj();
	}

	int date2doy(int day, int month);

	std::pair<int, int> doy2date(int doy);

}

#endif // HELPER_H
