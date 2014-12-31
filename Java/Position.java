
public class Position {
	public char x;
	public char y;
	
	public Position(char x, char y){
		this.x = x;
		this.y = y;
	}
	
	public void setX(char x){
		this.x = x;
	}
	
	public char getX(){
		return this.x;
	}
	
	public void setY(char y){
		this.y = y;
	}
	
	public char getY(){
		return this.y;
	}
	
	public String toString(){
		return String.valueOf(x) + String.valueOf(y);
	}
}
