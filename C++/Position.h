
class Position{
	char x;
	char y;
public:
	Position():x(' '), y(' '){}
	Position(char a, char b): x(a), y(b){}
	
	void setX(char a){ x = a; }
	char getX() const{ return x; }
	void setY(char b){ y = b; }
	char getY() const{ return y; }
	
	Position& operator=(const Position& p){
		x = p.getX();
		y = p.getY();
		
		return *this;
	}
};