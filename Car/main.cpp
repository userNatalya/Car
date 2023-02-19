#include<iostream>
#include<conio.h>
#include<thread>
#include<Windows.h>
using namespace std;
using namespace std::literals::chrono_literals;
//#define MIN_TANK_VOLUME 20//��������� ����������� � �(� ���������� ���������� ������)����� const int
//#define MAX_TANK_VOLUME 80
//#define TANK_CHECK
//#define TANK_CHECK
//��������� �������� ���� ������ ���,� �� ���� �������� ��������,//����� ������� ���������� �� � ����������� �������� ���������� #ifdef...#dif
//#define ENGINE_CHECK
//# ����������,��� ������ ���� ��������� 
//��������� -��� �������� �����������  �� ���������� ��������� ��������
//��������,��������� #include(�������� � ������,����������) ��������� ����������� �� �� ,��� � ������ ��������� ����� ����� ���������� ������ ����
//��������� #define(����������)������� ���������������� ������,���� ��� ��������
//� ����� ��� ���������� ����� ��� ������� ��������� �������� �������

#define Enter 13
#define Escape 27
class Tank//��������� ��� �������
{
	static const int MIN_TANK_VOLUME = 20;//����� ����.
	static const int MAX_TANK_VOLUME = 80;//������� �������,����������� ���������� �������� �� � �������,��� ��� ������ ��� ���� �������� ������,��� ����� ������

	const int VOLUME;//��� ������� ���������� ������
	double fuel_level;
public://����� geteri � setteri
	int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	Tank(int volume) :VOLUME
	(
		volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
		volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
		volume
	)
	{
		/*this->VOLUME = volume;*/
		this->fuel_level = 0;
		cout << "TConstuctor:\t" << this << endl;
	}
	~Tank()
	{
		cout << "TDestuctor:\t" << this << endl;
	}
	void fill(double amount)//��������� �������� ������� �������
	{
		if (fuel_level + amount > VOLUME)fuel_level = VOLUME;
		else fuel_level += amount;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	void info()const//����� �����.��� ����� ��������� ������ ����
	{
		cout << "Volume: \n " << VOLUME << endl;
		cout << "Fuel level:" << fuel_level << endl;
	}
};
class Engine
{
	static const int MIN_ENGINE_CONSUMPTION = 3;
	static const int MAX_ENGINE_COMSUMPTION = 30;
	const double CONSUMPTION;
	double consumption_per_second;
	bool is_started;
public:
	double get_CONSUMPTION()const
	{
		return this->CONSUMPTION;
	}
	double get_consumption_per_second()const
	{
		return this->consumption_per_second;
	}
	Engine(int consumption) :CONSUMPTION
	(
		consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption > MAX_ENGINE_COMSUMPTION ? MAX_ENGINE_COMSUMPTION :
		consumption
	)
	{
		consumption_per_second = CONSUMPTION * 3e-5;
		is_started = false;
		cout << "EConstructor:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Edestructor:\t" << this << endl;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		cout << "Consumption per 100 km:  " << CONSUMPTION << " liters\n";
		cout << "Consumption per 1 second:" << consumption_per_second << " liters\n";
	}
};
class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;

	int speed;
	static const int MAX_SPEED_LOW_LIMIT = 90;
	static const int MAX_SPEED_HIGH_LIMIT = 390;
	const int MAX_SPEED;
	struct Control
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}threads;

public:
	Car(int volume, int consumption, int max_speed = 250) :tank(volume), engine(consumption),
		MAX_SPEED
		(
			max_speed< MAX_SPEED_LOW_LIMIT ? MAX_SPEED_LOW_LIMIT :
			max_speed>MAX_SPEED_HIGH_LIMIT ? MAX_SPEED_HIGH_LIMIT :
			max_speed

		)
	{
		driver_inside = false;
		speed = 0;
		cout << "���� ������ ������,��������� " << endl;
	}
	~Car()
	{
		cout << "Car is over" << endl;
	}
	void panel()const
	{
		HANDLE hConsol = GetStdHandle(STD_OUTPUT_HANDLE);
		while (driver_inside)
		{
			system("CLS");

			cout << "Fuel level:\t" << tank.get_fuel_level() << " liters\t";
			if (tank.get_fuel_level() < 5)
			{
				SetConsoleTextAttribute(hConsol, 0xCF);//0x�F:
				cout << "LOW FUEL";//0X	CF "C'-������� ���,"F"-����� �����
				SetConsoleTextAttribute(hConsol, 0x07);//0x�F:
			}
			cout << endl;
			cout << "Engine " << (engine.started() ? "started" : "stopped") << "\n";
			cout << "Speed:" << speed << "\n";
			std::this_thread::sleep_for(1s);
		}

	}
	void info()const
	{
		tank.info();
		engine.info();
	}


	//������:
	void get_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);

	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		system("CLS");
		cout << "You are out of your car" << endl;
	}
	void start_engine()
	{
		if (driver_inside && tank.get_fuel_level())
		{
			engine.start();
			threads.engine_idle_thread = std::thread(&Car::engine_edle, this);
		}

	}
	void stop_engine()
	{
		if (driver_inside)
		{
			engine.stop();
			if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
		}
	}
	void accellerate()
	{

		if (engine.started() && speed < MAX_SPEED)
		{
			speed += 10;
			if (speed > MAX_SPEED)speed = MAX_SPEED;
			std::this_thread::sleep_for(500ms);
		}
		if (!threads.free_wheeling_thread.joinable())
			threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
	}
	void slow_down()
	{
		if (speed > 0)
		{
			speed -= 10;

			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(500ms);
		}
	}

	void control()
	{
		char key = 0;
		do
		{
			key = 0;
			if (_kbhit())key = _getch();

			/*key = _getch();*/
			switch (key)
			{
			case 'W':case 'w':
				if (engine.started() && speed < MAX_SPEED)
				{
					speed += 10;
					if (speed > MAX_SPEED)speed = MAX_SPEED;
					std::this_thread::sleep_for(1s);
					if (!threads.free_wheeling_thread.joinable())
						threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
				}
				break;
			case 'S':case 's':
				if (speed > 0)
				{
					speed -= 10;

					if (speed < 0)speed = 0;
					std::this_thread::sleep_for(500ms);
				}
				break;
			case Enter:driver_inside ? get_out() : get_in(); break;
			case'I':case'i'://���������
				engine.started() ? stop_engine() : start_engine();
				break;
			case'F':case'f':
				if (driver_inside)cout << "�� ����� �� ������!" << endl;
				else
				{
					double amount;
					cout << "������� ����� �������:"; cin >> amount;
					tank.fill(amount);
				}
				break;
			case Escape:
				stop_engine();
				get_out();
				break;
			}
			if (tank.get_fuel_level() == 0)stop_engine();
			if (speed <= 0)speed = 0;
			if (speed == 0 && threads.free_wheeling_thread.joinable())threads.free_wheeling_thread.join();
		} while (key != Escape);


	}
	void engine_edle()//�������� ��� ���������
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))//��� ����� ������ ����� ����� �������
		{
			std::this_thread::sleep_for(1s);
		}
	}
	void free_wheeling()
	{
		while (speed-- > 0)std::this_thread::sleep_for(1s);
		{
			if (speed < 0)speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}

};
//#define TANK CHECK
//��������� �������� ���� ������ ���,�� ���� �������� ��������,
//����� ������� ������������ � ����������� �������� ���������� #ifdef......... #endif

void main()
{
	setlocale(LC_ALL, "");

#if defined TANK_CHECK
	Tank tank(40);
	tank.info();
	do
	{
		int fuel;
		cout << "������� ����� �������"; cin >> fuel;
		tank.fill(fuel);
		tank.info();

	} while (_getch() != 27);//���� ���������� TANK_CHECK,�� ��������� ��� �� ��������� #endif ����� ����� �����������
#endif

#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHECK
	Car bmw(80, 12);
	bmw.info();
	bmw.control();
}
