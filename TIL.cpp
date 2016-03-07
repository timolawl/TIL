/* Today I Learned
 *
 * The purpose of this program is to provide a history of things learned by me. Each day I will enter in what I have learned.
 *
 *	Functions:
 *		display_menu() - displays the menu with the various options operating on a switch statement. display_menu() calls
 *						 itself if none of the viable options were selected.
 *
 *  Goals: 
 *		[x] Incorporate a cipher into the program so that even if the initial password is bypassed, user will still require the cipher to decode stored entries.
 *		[ ] Incorporate basic text editing capabities into the program through a header file/class definition.
 *		[ ] Turn Entries into a subprogram to a larger program. Second item on the larger program can be anything, say, a calculator.
 *		[ ] Introduce a game (perhaps a card game).
 *
 */

#include <iostream>
#include <iomanip> 					// For setw.
#include <cstdlib> 					// This library is for the exit function, exit(EXIT_FAILURE); also for atoi, which you didn't use.
#include <string>
#include <sstream>
#include <fstream>
#include <cctype> 					// why is this not needed in using isalpha, isdigit?
#include <unistd.h>					// for making a getch() and disenable/renable echo
#include <termios.h>  				// for making a getch() and disenable/renable echo
#include <bitset>
#include <cmath> 					// for pow(b, e)
#include <ctime>
#include <vector>
#include "entry.h"

using namespace std;
/*
class entry {			//should we have a running count of size at each entry so that we can know where to delete immediately? it calls for reassignment of running count, and entry#.
private:				//the reason for this is because it may be easy to delete on the map (by entry#), but you sEntriesl won't know where to delete on the file unless you 
	string title;
	string date;
	string text;

public:
	entry(string date, string title, string text) {	// this constructor for retrieving old entries.
		this->date = date;
		this->title = title;
		this->text = text;
	}

	int getSize() {	//this is for when you want to delete something and need the size of the deleted entry to shift the entries properly
		return this->title.size() + 10 + this->text.size();
	}

	string getDate() {
		return date;
	}

	string getTitle() {
		return title;
	}

	string getText() {
		return text;
	}

};
*/

void display_menu(fstream &fs, vector<entry> &coded_entries, vector<entry> &uncoded_entries);
void populate_map(fstream &fs, string &nameBuffer, vector<entry> &coded_entries, vector<entry> &uncoded_entries);
void readEntries(vector<entry> &coded_entries, vector<entry> &uncoded_entries);
void writeEntries(fstream &fs, vector<entry> &coded_entries, vector<entry> &uncoded_entries);
string serializeSize(const int &size);
void getCipher(string &pass);
string cipher(string str, const string &pass); //, bool rwstate);
void checkPassword();
int getch();
string obtainCurrentDate();

const int LINE_MAX = 80;
const int LINE_BEGIN = 10;
const int DATE_WIDTH = 10;
const int MAX_PASS = 50;
const int MAX_ATTEMPTS = 3;
const int MAX_BITS = 8;
//const bool READ = true;
//const bool WRITE = false;

int main() {
	
	string password, key, binary_file, name, nameSize, encryptedName, encryptedNameSize;
	fstream filestream;															// declaring an input/output file stream.
	vector<entry> ciphered_entries, deciphered_entries;

	checkPassword();

	cout << "\nWelcome to Today I Learned (TIL), a program that keeps track of daily learnings." << endl;
	cout << "What is the name of the file you wish to use? ";
	getline(cin, binary_file);
	if(binary_file.find(".bin") == string::npos)
		binary_file += ".bin";
	filestream.open(binary_file.c_str()); 				
	if(!filestream.is_open()) {													// This line checks if it is opened or not.
		filestream.open(binary_file.c_str(), ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc); // If not opened (i.e., doesn't exist), then one is created.
		cout << "Since this is the beginning of a new TIL journal, who does this journal belong to? \nEnter name: ";
		//cin >> name;		// this is if you want to get names without spaces. if you have whitespace it will not be picked up and remain in the stream buffer
		//cin.ignore();		// there is also no need for cin.ignore() to follow getline...
		getline(cin, name);

		nameSize = serializeSize(name.size());

		//set it up so that the sizes and names are always encrypted with Crypto, and everything else is personally encrypted.
		encryptedName = cipher(name, "Crypto");
		encryptedNameSize = cipher(nameSize, "Crypto");

		filestream.write(&encryptedNameSize[0], 5);
		filestream.write(&encryptedName[0], name.size());

		cout << "\nGreetings, " << name << "." << endl;	
	}
	else {
		populate_map(filestream, name, ciphered_entries, deciphered_entries);
		cout << "\nWelcome back, " << name << "." << endl;
		cout << "\nPopulation of map successful." << endl;
	}
																				// what does trunc do? - destroys the old file if it exists
																				// apparently ios_base::binary -> data is in binary form.
																				// ios_base is a superset of ios, but it seems like ios_base == ios...
	
	display_menu(filestream, ciphered_entries, deciphered_entries);

	filestream.close();

	return 0;
}


void display_menu(fstream &fs, vector<entry> &coded_entries, vector<entry> &uncoded_entries) { //, string &key) {

	char option;

	cout << "\nMain Menu: " << endl;
	cout << "\t(R) Display past entries." << endl;
//	cout << "\t(D) Display details of specific Entries." << endl;
	cout << "\t(N) Display the number of past entries." << endl;
	cout << "\t(W) Write an entry." << endl;
//	cout << "\t(E) Edit a previous Entries entry." << endl; 				// don't know about this option. shouldn't be able to edit it. it's like writing in a journal.
	cout << "\t(Q) Quit." << endl;
	cout << "What would you like to do? ";

	//

	cin >> option;
	cin.ignore();									// this is needed to make sure that getline doesn't pick up the '\n' that cin >> leaves behind. // set(w) doesn't seem to get rid of it too.
	option = toupper(option);
	cout << endl;

	switch(option) {
		case 'R': 
			if(coded_entries.empty()) {
				cout << "There is no entry to display." << endl;
			}
			else readEntries(coded_entries, uncoded_entries);
			break;
//		case 'D': detailEntries(fs); break;
/*		case 'E': eraseEntries (fs, key); break; */
		case 'N': 
			if(coded_entries.empty()) {
				cout << "There is currently 0 entries." << endl;
			}
			else if(coded_entries.size() == 1) {
				cout << "There is currently 1 entry." << endl;
			}
			else {
				cout << "There are currently " << coded_entries.size() << " entries." << endl;
			}
			break;
		case 'W': writeEntries(fs, coded_entries, uncoded_entries); break;
		case 'Q': return; //{ cout << endl; exit(EXIT_FAILURE); }  				// exits the program.
	//	case 'Z': getCipher(key); break;
		default: cout << "Invalid entry. Please try again:\n";
	}
	display_menu(fs, coded_entries, uncoded_entries);
}


void populate_map(fstream &fs, string &name, vector<entry> &coded_entries, vector<entry> &uncoded_entries) {
	string sizeBuffer, nameBuffer, titleBuffer, dateBuffer, textBuffer;	//do I need to include room for the null character? is the null character present in the file?
	int size_of_string, entry_number;													//can i use a c++ string in place of the c-style string?

//	entry temp_stored_entry;
//	stringstream oss (ios_base::in | ios_base::out | ios_base::binary);
//	osstream << fs.rdbuf(); 

//	fs.seekg(0, fs.end);
//	int length = fs.tellg();
//	cout << "Length of file: " << length << endl;

	fs.seekg(0, fs.beg);												// returns the fstream pointer to the beginning.

	//read the name first
	sizeBuffer = "xxxxx";
	fs.read(&sizeBuffer[0], 5);
	istringstream iss(cipher(sizeBuffer, "Crypto"));
	iss >> size_of_string;
	iss.clear();

//	cout << sizeBuffer << " " << size_of_string << endl;

	nameBuffer.clear();
	for(int i = 0; i < size_of_string; i++) {
		nameBuffer += 'x';
	}
//	cout << nameBuffer << endl;
//	cout << "testing" << endl;

	fs.read(&nameBuffer[0], size_of_string);

	name = cipher(nameBuffer, "Crypto");

//	cout << nameBuffer << endl;

	while(fs.peek() != EOF) {
		sizeBuffer = "xxxxx"; //initializing to a size of 5.	// so apparently this is necessary when reading into strings. they must be initialized for size.

		//read the size buffer for title
		fs.read(&sizeBuffer[0], 5);
		//convert the sizeBuffer into numeric form.
		iss.str(cipher(sizeBuffer, "Crypto"));
		iss >> size_of_string;
		iss.clear();

//		cout << sizeBuffer << endl;

	//	if(!fs) {
	//		cout << fs.gcount() << " characters read.";
	//		fs.clear();
	//	}

	//	string titleBuffer(size_of_string, 'x');
		titleBuffer.clear();
		for(int i = 0; i < size_of_string; i++) {
			titleBuffer += 'x';
		}

		//read the title into buffer based on determined size
		fs.read(&titleBuffer[0], size_of_string);

		dateBuffer = "xxxxxxxx";
		//read the date - always size of ten characters
		fs.read(&dateBuffer[0], 8);

		//read the size buffer for text
		sizeBuffer.clear();
		sizeBuffer = "xxxxx";
		fs.read(&sizeBuffer[0], 5);
		//convert the sizeBuffer into numeric form.
		iss.str(cipher(sizeBuffer, "Crypto"));						// apparently this is how you reuse the stream. clear() only removes the error flag state.
		iss >> size_of_string;

		textBuffer.clear();
		for(int i = 0; i < size_of_string; i++) {
			textBuffer += 'x';
		}
		
		fs.read(&textBuffer[0], size_of_string);

		//compile data into entry object:
		entry temp_stored_entry = entry(dateBuffer, titleBuffer, textBuffer);

		//import to encrypted map:
		coded_entries.push_back(temp_stored_entry);
	}
	uncoded_entries = coded_entries;								// copy maps over (does this work?)
	fs.clear();															// this clears the EOF flag
	fs.seekg(0, fs.beg);												// returns the fstream pointer to the beginning.

}

void readEntries(vector<entry> &coded_entries, vector<entry> &uncoded_entries) {
	char option;
	string title, text, word, blank, pass, decipheredDate, decipheredTitle, decipheredText;
	int position, current_position, entry_number;

	for(int i = 0; i < uncoded_entries.size(); i++) {
		//print out date
		cout << left << setw(DATE_WIDTH) << uncoded_entries.at(i).GetDate();

		//print out title unless it isnt there.
		title = uncoded_entries.at(i).GetTitle();
		if(!title.empty()) {
			if(title.size() <= LINE_MAX) {
				cout << title << endl << endl;
			}
			else {
				current_position = 0;
				position = LINE_BEGIN;
				word.clear();

				while(!title.empty()) {
					if(!isspace(title.at(0))) {
						word += title.at(0);
						title.erase(0,1);
						current_position++;
						if(position + current_position >= LINE_MAX) {
							cout << endl << setw(DATE_WIDTH) << blank; 		// sets the next line appropriately
							position = LINE_BEGIN;
						}
						// bring to new line
					} 
					else {
						cout << word << title.at(0);
						word.clear();
						title.erase(0,1);
						position += current_position;
						if(position + current_position >= LINE_MAX) {
							cout << endl << setw(DATE_WIDTH) << blank; 		// sets the next line appropriately
							position = LINE_BEGIN;
						}
						position++;
						current_position = 0;
					}
				}
				cout << word << endl << endl; 	// to make sure the last word gets added and to keep the display neat.
			}
		}
		
		cout << left << setw(DATE_WIDTH) << blank;

		//print out text
		text = uncoded_entries.at(i).GetText();
		if(!text.empty()) {
			if(text.size() <= LINE_MAX) {
				cout << text << endl << endl;
			}
			else {

				current_position = 0;
				position = LINE_BEGIN;
				word.clear();

				while(!text.empty()) {
					if(!isspace(text.at(0))) {
						word += text.at(0);
						text.erase(0,1);
						current_position++;
						if(position + current_position >= LINE_MAX) {
							cout << endl << setw(DATE_WIDTH) << blank; 		// sets the next line appropriately
							position = LINE_BEGIN;
						}
						// bring to new line
					} 
					else if(text.at(0) != '\n') {
						cout << word << text.at(0);
						word.clear();
						text.erase(0,1);
						position += current_position;
						if(position + current_position >= LINE_MAX) {
							cout << endl << setw(DATE_WIDTH) << blank; 		// sets the next line appropriately
							position = LINE_BEGIN;
						}
						position++;
						current_position = 0;
					}
					else {
						cout << word << text.at(0);
						word.clear();
						text.erase(0,1);
						cout << endl << setw(DATE_WIDTH) << blank;
						position = LINE_BEGIN;
						current_position = 0;
					}
				}
				cout << word << endl << endl; 	// to make sure the last word gets added and to keep the display neat.
			}
		}
	}

	//display read options: (this can be made into its own function to be loop called instead of directing back to the main menu)

	cout << "Read Menu: " << endl;
	cout << "\t(1) Decipher all entries using a cipher." << endl;
	cout << "\t(2) Decipher a specific entry using a cipher." << endl;
	cout << "\t(3) Return to Main Menu." << endl;
	cout << "What would you like to do? ";

	cin >> option;
	cin.ignore();									// this is needed to make sure that getline doesn't pick up the '\n' that cin >> leaves behind. // set(w) doesn't seem to get rid of it too.
	cout << endl;

	switch(option) {
		case '1': 
			{
				getCipher(pass);
				for(int i = 0; i < coded_entries.size(); i++) {
					decipheredDate = cipher(coded_entries.at(i).GetDate(), pass);
					decipheredTitle = cipher(coded_entries.at(i).GetTitle(), pass);
					decipheredText = cipher(coded_entries.at(i).GetText(), pass);
					entry replacement_entry = entry(decipheredDate, decipheredTitle, decipheredText);
					uncoded_entries.at(i) = replacement_entry;	// do i have to do anything with the class, or is simple copying okay?
				}
			}
			break;
		case '2': 
			{
				getCipher(pass);
				cout << "which entry would you like to decipher?" << endl;
				cin >> entry_number;
				cin.ignore();
				decipheredDate = cipher(coded_entries.at(entry_number - 1).GetDate(), pass);
				decipheredTitle = cipher(coded_entries.at(entry_number - 1).GetTitle(), pass);
				decipheredText = cipher(coded_entries.at(entry_number - 1).GetText(), pass);
				entry replacement_entry = entry(decipheredDate, decipheredTitle, decipheredText);
				uncoded_entries.at(entry_number - 1) = replacement_entry;
			}
			break;
		case '3': 
			return;
			break;
		default: cout << "Invalid entry. You have been returned to the Main Menu\n\n";
	}
}


void writeEntries(fstream &fs, vector<entry> &coded_entries, vector<entry> &uncoded_entries) {
	string title, text, paragraph, pass, date, titleSize, textSize, encryptedTitle, encryptedTitleSize, encryptedText, encryptedTextSize, encryptedDate;
	vector<string> paragraphs;
//	char key_character1, key_character2, letter;
	int total_entry_size;
//	entry new_entry;

	fs.seekp(0, fs.end);					// place the position of the put pointer to the end, offset by 0. The put pointer means the writing pointer. No need for ::app (append)
	
	//get title
	cout << "Entry Title (press Enter and Enter again at the entry prompt to return to the main menu): " << endl; //what if i don't want a title?
	getline(cin, title);
	
	//get entry
	cout << endl << "Entry Body (when finished, press Enter three times):" << endl;
	getline(cin, paragraph);

	if(paragraph.empty()) {
		return;
	}

	while(!paragraph.empty()) {
		paragraphs.push_back(paragraph);
		cin.ignore();	//this is the ignore for the initial enter.
		//cin.ignore();
		getline(cin, paragraph);
	}

	for(int i = 0; i < paragraphs.size(); i++) {
		text += paragraphs.at(i);
		text += "\n";
	}

/*	key_character1 = cin.get();
	if(title.empty() && (key_character1 == '\n')) return;

	text.clear();
	text += key_character1;
	key_character2 = key_character1;
	key_character1 = cin.get();
	text += key_character1;
	letter = cin.get();

	while(key_character1 != '\n' && key_character2 != '\n' && letter != '\n') {	//i suppose i can use newline, but that means i won't be able to modify previous paragraphs.
		text += letter;
		key_character1 = letter;
		key_character2 = key_character1;
		letter = cin.get();
	}
*/
	
	//obtain date for entry:
	date = obtainCurrentDate();

	//get and apply cipher
	getCipher(pass);
	encryptedTitle = cipher(title, pass); //, WRITE);
	encryptedDate = cipher(date, pass); //, WRITE);
	encryptedText = cipher(text, pass); //, WRITE);

	//update maps:
	entry new_entry = entry(encryptedDate, encryptedTitle, encryptedText);
	coded_entries.push_back(new_entry);
	uncoded_entries.push_back(new_entry);

	//serialize title and text sizes:
	titleSize = serializeSize(title.size());
	textSize = serializeSize(text.size());

	encryptedTitleSize = cipher(titleSize, "Crypto");
	encryptedTextSize = cipher(textSize, "Crypto");

	//write contents to file
	fs.write(&encryptedTitleSize[0], 5);
	fs.write(&encryptedTitle[0], title.size());
	fs.write(&encryptedDate[0], 8);	//size of date
	fs.write(&encryptedTextSize[0], 5);
	fs.write(&encryptedText[0], text.size());

	fs.seekg(0, fs.beg); 					// apparently this is needed because the seekg and seekp use the same pointer. I wasn't able to access the listings without doing this.
}

string serializeSize(const int &size) {
	ostringstream oss;
	string serializedSize;

	if(size < 1) {
		serializedSize = "00000";
	}
	else if(size < 9) {
		oss << size;
		serializedSize = "0000" + oss.str();
	}
	else if(size < 99) {
		oss << size;
		serializedSize = "000" + oss.str();
	}
	else if(size < 999) {
		oss << size;
		serializedSize = "00" + oss.str();
	}
	else if(size < 9999) {
		oss << size;
		serializedSize = "0" + oss.str();
	}
	else if(size < 99999) {
		oss << size;
		serializedSize = oss.str();
	}
	else cout << endl << "There seems to be an error with the title size.." << endl;

	return serializedSize;

}

void getCipher(string &pass) {		// get the cipher
	string compare;
	
	while(true) {
		pass.clear();
		compare.clear();
		cout << "Enter cipher: ";
		for(int i = 0; i < MAX_PASS; i++) {
			pass += getch();
			if(pass[i] == '\n' || pass[i] == '\r') {
				pass.erase(i, 1);
				break;
			}
		}
		cout << endl << "Confirm cipher: ";
		for(int j = 0; j < MAX_PASS; j++) {
			compare += getch();
			if(compare[j] == '\n' || compare[j] == '\r') {
				compare.erase(j, 1);
				break;
			}
		}
		if(pass != compare)
			cout << endl << "Error: Ciphers do not match. Please try again." << endl;
		if(pass == compare) {
			cout << endl << endl;
			break;
		}
	}
}

string cipher(string str, const string &pass) { //, bool rwstate) { // when encountering a '\n', reset cpos. but only when reading, not writing. 
	// str -> bin -- convert --> c.bin -> c.str (return this)
	// same thing backward
	// c.str -> c.bin -- convert --> bin -> str (return this)
	// declarations
	bitset<MAX_BITS> binary, cipher, converted;
	string cipher_string, binary_string, converted_string;
	int cpos, converted_bit, bit_counter, ascii_num;
	char ascii_char;
	// check if getCipher has been initiated. If not, then it simply returns the normal string.
	if(pass.empty())
		return str;
	// convert cipher string to binary form; store in string.
	for(int i = 0; i < pass.size(); i++) {
		cipher = pass[i];
		cipher_string += cipher.to_string();
	}
	// convert string to binary form; store binary form in string.
	for(int j = 0; j < str.size(); j++) {
		binary = str[j];
		binary_string += binary.to_string();
	}
	// apply cipher
	for(int k = 0, cpos = 0, bit_counter = 1; k < binary_string.size(); k++, cpos++, bit_counter++) {
		if(cpos == cipher_string.size())
			cpos = 0;
		// convert one character in binary and cipher to it's numerical value at a time, stringstream or atoi; apply XOR function
		converted[MAX_BITS - bit_counter] = binary_string[k] ^ cipher_string[cpos];
	// convert back to string of ascii characters.
		if(bit_counter == MAX_BITS) {
			bit_counter = 0;
			ascii_num = 0;
			for(int a = 0; a < MAX_BITS; a++) {
				if(converted[a] == 1)
					ascii_num += pow(2, a);
			}
			ascii_char = ascii_num;
	//		if(rwstate) 
	//			if(ascii_char == '\n')
	//				cpos = -1;

			converted.reset(); // probably don't need this but i suppose on the safe side?
			converted_string += ascii_char;
		}
				//	binary_string[k] ^= cipher_string[cpos]; // don't think this works because it's in string form... i believe it has to be in int form or bitset form.
	}
	return converted_string;
}

void checkPassword() {					// retrieves initial password to gain access to the program.
	string password;
	for(int i = 1; i <= MAX_ATTEMPTS; i++) {
		password = "";
		cout << "Please enter the password: ";
		for(int j = 0; j < MAX_PASS; j++) {
			password += getch();	//getch does not echo to screen, which is why it is used.
			if(password[j] == '\n' || password[j] == '\r') {
				password.erase(j, 1);						// -= '\n'; // this doesn't exist in the string class
				break;
			}
		}
		if((password == "Crypto")) return; // XOR better but no time right now..
		else {
			if(i == MAX_ATTEMPTS) {
				cout << endl << "Incorrect password. Good bye." << endl;
				exit(EXIT_FAILURE);
			}
			if(i == 2) cout << endl << "Incorrect password. " << MAX_ATTEMPTS - i << " attempt remaining. " << endl;
			else cout << endl << "Incorrect password. " << MAX_ATTEMPTS - i << " attempts remaining. " << endl;
		}
	}
}

int getch() {							// getch() does not exist naturally on *nix. This was created for the ability to turn off echo.
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

string obtainCurrentDate() {
	string year, month, day, date;		

	time_t t = time(0);
	struct tm * now = localtime(&t);

	ostringstream oss;
	oss << (now->tm_year - 100);
	year = oss.str();

	if((now->tm_mon + 1) < 10) {
		oss.str("");
		oss << '0' << now->tm_mon + 1;
		month = oss.str();
	}
	else {
		oss.str("");
		oss << now->tm_mon;
		month = oss.str();
	}

	if(now->tm_mday < 10) {
		oss.str("");
		oss << '0' << now->tm_mday;
		day = oss.str();
	}
	else {
		oss.str("");
		oss << now->tm_mday;
		day = oss.str();
	}

	date.append(month).append("-").append(day).append("-").append(year);

	return date;
}
