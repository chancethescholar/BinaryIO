#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Ship
{
public:
	static int ship_count;
	class Weapon
	{
	public:
		string name;
		int power_rating;
		float power_consumption;
	};
	string name;
	string ship_class;
	short length = 0;
	int shield_capacity;
	float warp_speed;
	int weapon_count = 0;
	Weapon * armaments = nullptr;
	int firepower;
};

//static variables need declarations
int Ship::ship_count;

string GetFileName(int file);
void LoadFile(int file, Ship * (&ships));
void Display(Ship * s);
void DisplayUnarmedShips(Ship * ships);
Ship * FindExtrema(Ship * ships, bool strongest);
Ship * FindStrongestWeapon(Ship * ships);

int main()
{
	Ship::ship_count = 0;
	cout << "Which file(s) to open?\n";
	cout << "1. friendlyships.shp" << endl;
	cout << "2. enemyships.shp" << endl;
	cout << "3. Both files" << endl;
	int option;
	cin >> option;

	Ship * ships = nullptr;
	LoadFile(option, ships);

	cout << "1. Print all ships" << endl;
	cout << "2. Starship with the strongest weapon" << endl;
	cout << "3. Strongest starship overall" << endl;
	cout << "4. Weakest ship (ignoring unarmed)" << endl;
	cout << "5. Unarmed ships" << endl;

	cin >> option;

	if (option == 1)
	{
		for (int i = 0; i < Ship::ship_count; i++)
		{
			Display(&ships[i]);
		}
	}
	else if (option == 2)
	{
		Display(FindStrongestWeapon(ships));
	}
	else if (option == 3)
	{
		Display(FindExtrema(ships, true));
	}
	else if (option == 4)
	{
		Display(FindExtrema(ships, false));
	}
	else if (option == 5)
	{
		DisplayUnarmedShips(ships);
	}
	return 0;
}

string GetFileName(int file)
{
	if (file > 3 || file < 0)
	{
		cout << "That there file don't exist, pardner";
		throw - 1;
	}
	string a = (file == 1 ? "friendly" : "enemy");
	return a + "ships.shp";
}

//The 2nd parameter is a reference to an array of ships. I forget why I did this.
//Returns an array of all the loaded ships
void LoadFile(int file, Ship * (&ships))
{
	if (file == 3)
	{
		LoadFile(1, ships);
		LoadFile(2, ships);
		return;
	}
	ifstream infile(GetFileName(file), ios_base::binary);
	//count is num of ships, size is temp variable
	unsigned int count, size;
	infile.read((char *)&count, 4);
	count += Ship::ship_count;
	int old_ship_count = Ship::ship_count;
	Ship::ship_count = count;
	//ships is the old array coming in.
	//new_ships is the new array that ships will be added to 
	Ship * new_ships = new Ship[count];

	//copy Ships from ships into new_ships
	for (int i = 0; i < old_ship_count; i++)
	{
		new_ships[i] = ships[i];
	}
	for (unsigned int i = old_ship_count; i < count; i++)
	{
		//reading ship name
		infile.read((char*)&size, 4);//taking size of name into size
		char* str = new char[size + 1];
		infile.read(str, size);//reading the name
		str[size] = '\0';
		new_ships[i].name = string(str);
		delete[] str;

		//reading ship class
		infile.read((char*)&size, 4);
		str = new char[size + 1];
		infile.read(str, size);
		str[size] = '\0';
		new_ships[i].ship_class = str;
		delete[] str;

		infile.read((char *)&new_ships[i].length, 2);
		infile.read((char *)&new_ships[i].shield_capacity, 4);
		infile.read((char *)&new_ships[i].warp_speed, 4);
		//reading how many weapons ship has
		infile.read((char *)&new_ships[i].weapon_count, 4);
		if (new_ships[i].weapon_count != 0)
		{
			new_ships[i].armaments = new Ship::Weapon[new_ships[i].weapon_count];
			int current_power = 0;
			for (int j = 0; j < new_ships[i].weapon_count; j++)
			{
				//for calculating ship power
				//reading weapon name
				infile.read((char*)&size, 4);
				str = new char[size + 1];
				infile.read(str, size);
				//the null terminator is the end of all strings
				str[size] = '\0';
				new_ships[i].armaments[j].name = str;
				delete[] str;

				infile.read((char *)&new_ships[i].armaments[j].power_rating, 4);
				infile.read((char *)&new_ships[i].armaments[j].power_consumption, 4);
				current_power += new_ships[i].armaments[j].power_rating;
			}
			new_ships[i].firepower = current_power;
		}
	}
	delete[] ships;
	ships = new_ships;
	infile.close();
}

//prints out all the details of the set at the index
void Display(Ship * s)
{
	cout << "Name: " << s->name << endl;
	cout << "Class: " << s->ship_class << endl;
	cout << "Length: " << s->length << endl;
	cout << "Shield capacity: " << s->shield_capacity << endl;
	cout << "Maximum Warp: " << s->warp_speed << endl;
	cout << "Armaments: " << endl;
	for (int i = 0; i < s->weapon_count; i++)
	{
		cout << s->armaments[i].name << ", " << s->armaments[i].power_rating;
		cout << ", " << s->armaments[i].power_consumption << endl;
	}
	if (s->armaments == nullptr)
	{
		cout << "Unarmed" << endl;
	}
	else
	{
		cout << "Total firepower: " << s->firepower << endl << endl;
	}
}

void DisplayUnarmedShips( Ship * ships )
{
	for (int i = 0; i < Ship::ship_count; i++)
	{
		if ( ships[i].armaments == nullptr )
		{
			Display(&ships[i]);
		}
	}
}

//Finds either the strongest (if strongest is true)
//or the weakest (is strongest is false) ship
Ship * FindExtrema(Ship * ships, bool strongest)
{
	int x = strongest ? 1 : -1;
	Ship * result = &ships[0];
	int current_extrema = ships[0].firepower;
	for (int i = 1; i < Ship::ship_count; i++)
	{
		//I have no idea how this code works but if you multiply
		//both sides of an inequality by -1, the sign flips
		if (ships[i].firepower > 0 && (x * ships[i].firepower > x * current_extrema))
		{
			current_extrema = ships[i].firepower;
			result = &ships[i];
		}
	}
	return result;
}

Ship * FindStrongestWeapon(Ship * ships)
{
	Ship * strongest_ship = &ships[0];
	int current_highest_power = 0;
	for (int i = 0; i < Ship::ship_count; i++)
	{
		//if array of weapons is null
		if (ships[i].armaments != nullptr)
		{
			for (int j = 0; j < ships[i].weapon_count; j++)
			{
				if (ships[i].armaments[j].power_rating > current_highest_power)
				{
					current_highest_power = ships[i].armaments[j].power_rating;
					strongest_ship = &ships[i];
				}
			}
		}
	}
	return strongest_ship;
}