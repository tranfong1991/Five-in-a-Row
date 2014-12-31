import java.awt.Image;
import javax.swing.ImageIcon;
import javax.swing.JButton;

enum PieceType {
	CROSS(new ImageIcon("Images/X_Icon.jpg")), 
	CIRCLE(new ImageIcon("Images/O_Icon.jpg"));

	ImageIcon icon;

	PieceType(ImageIcon i) {
		Image img = i.getImage().getScaledInstance(30, 30, Image.SCALE_SMOOTH);
		icon = new ImageIcon(img);
	}

	ImageIcon getIcon() {
		return icon;
	}
}

public class Piece extends JButton {
	private Position position;
	private PieceType type;
	private boolean clicked;

	public Piece(char x, char y) {
		super();
		this.clicked = false;
		this.position = new Position(x,y);
	}
	
	public boolean isClicked() {
		return clicked;
	}

	public void click() {
		if (clicked)
			return;

		this.clicked = true;
		this.setIcon(type.getIcon());
		this.update(this.getGraphics());
	}
	
	public void unclick(){
		this.clicked = false;
		this.setIcon(null);
	}
	
	public PieceType getPieceType(){
		return this.type;
	}
	
	public void setPieceType(PieceType type){
		if(type == null)
			return;
		this.type = type;
	}
	
	public void setPosition(char x, char y){
		this.position.setX(x);
		this.position.setY(y);
	}
	
	public Position getPosition(){
		return this.position;
	}
	
	public void disable(){
		setEnabled(false);
	}
}
