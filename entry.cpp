#include <string>
#include "entry.h"

entry::entry() {}
//entry::~entry() {}

entry::entry(std::string date, std::string title, std::string text) : date(date), title(title), text(text) {	// so using an initialization list is standard, and this assignment actually works.
																			// this constructor for retrieving old entries.
//	this->date = date;
//	this->title = title;
//	this->text = text;
}

int entry::GetSize() const {	//this is for when you want to delete something and need the size of the deleted entry to shift the entries properly
	return this->title.size() + 8 + this->text.size();
}

std::string entry::GetDate() const {
	return date;
}

std::string entry::GetTitle() const {
	return title;
}

std::string entry::GetText() const {
	return text;
}