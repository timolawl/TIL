#ifndef _entry_h
#define _entry_h

#include <string>

class entry {			//should we have a running count of size at each entry so that we can know where to delete immediately? it calls for reassignment of running count, and entry#.
private:				//the reason for this is because it may be easy to delete on the map (by entry#), but you sEntriesl won't know where to delete on the file unless you 
	std::string title;
	std::string date;
	std::string text;

	entry();
//	~entry();

public:
	entry(std::string date, std::string title, std::string text);

	int GetSize() const;

	std::string GetDate() const;

	std::string GetTitle() const;

	std::string GetText() const;

};

#endif