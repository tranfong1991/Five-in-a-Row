
public class Util {
	public static char intToChar(int i){
		if(i<10)
			return Character.forDigit(i, 10);
		
		switch(i){
		case 10:
			return 'a';
		case 11: 
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		default:
			return 'f';
		}
	}
	
	public static int charToInt(char c){
		if(c == 'a' || c== 'b' || c== 'c' || c=='d' || c == 'e')
			return c -'a' + 10;
		return c - '0';
	}
	
	public static boolean isHex(char c) {
		if (Character.isDigit(c)) return true;
		if (c <= 'f' && c >= 'a') return true;
		if (c <= 'F' && c >= 'A') return true;
		return false;
	}
	
	public static boolean isMove(String s) {
		// 1 or xy is not a valid move
		if(s.length() < 2 || !isHex(s.charAt(0)) || !isHex(s.charAt(1))) return false;
		// abc is not a valid move
		if(s.length() > 2 && Character.isLetterOrDigit(s.charAt(2))) return false;
		return true;
	}
}
