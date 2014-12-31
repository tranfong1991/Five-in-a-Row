import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;

import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class Board extends JPanel implements ActionListener {
	private Piece[][] pieces = new Piece[15][15];
	private BufferedReader in;
	private PrintStream out;
	private boolean boardLocked;
	private boolean AItoggle;

	public Board(BufferedReader in, PrintStream out) {
		setLayout(new GridLayout(15, 15));
		setVisible(true);

		this.in = in;
		this.out = out;
		this.boardLocked = false;

		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				Piece p = new Piece(Util.intToChar(i), Util.intToChar(j));
				p.addActionListener(this);
				pieces[i][j] = p;
			}
		}

		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				add(pieces[i][j]);
			}
		}
	}
	
	
	public boolean isLocked(){
		return this.boardLocked;
	}
	
	public void setLock(boolean b){
		this.boardLocked = b;
	}
	
	public void setToggle(boolean b){
		this.AItoggle = b;
	}
	
	public Piece[][] getBoard() {
		return this.pieces;
	}

	public void click(int x, int y) {
		pieces[x][y].click();
		out.print("MOVE " + pieces[x][y].getPosition().toString());
		try {
			executeCommand(in.readLine(), pieces[x][y]);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void unclick(int x, int y){
		pieces[x][y].unclick();
	}
	public void reset() {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				pieces[i][j].unclick();
			}
		}
	}

	/*
	 * -1: special situations (i.e. board filled, AI 1 won, AI 2 won)
	 * 1: normal situations
	 */
	public int executeAIAI(String command){
		System.out.println(command);
		if (Util.isMove(command)) {
			int x = Util.charToInt(command.charAt(0)) - 1;
			int y = Util.charToInt(command.charAt(1)) - 1;
			
			if(AItoggle) pieces[x][y].setPieceType(PieceType.CROSS);
			else   		 pieces[x][y].setPieceType(PieceType.CIRCLE);
			AItoggle = !AItoggle;
			pieces[x][y].click();
			if(command.length() > 3) { 
				if(command.substring(3).equals("; Other AI Won!")) {	
					JOptionPane.showMessageDialog(this, "Oops! Opponent AI Won.");
					pieces[x][y].update(pieces[x][y].getGraphics());
					return -1;
				} else if (command.substring(3).equals("; We Won!")) {
					JOptionPane.showMessageDialog(this, "Congratulations! Our AI Won.");
					pieces[x][y].update(pieces[x][y].getGraphics());
					return -1;
				}
			}
		}
		return 1;
	}
	private void executeCommand(String command, Object obj) {
		System.out.println(command);
		if (Util.isMove(command)) {
			int x = Util.charToInt(command.charAt(0)) - 1;
			int y = Util.charToInt(command.charAt(1)) - 1;
			
			pieces[x][y].setPieceType(PieceType.CROSS);
			pieces[x][y].click();
			boardLocked = false;
			if(command.length() > 3 && command.substring(3).equals("; AI Won!")) {
				JOptionPane.showMessageDialog(this, "Oops! Opponent Won.");
				boardLocked = true;
			}
		} else if (command.equals("; Illegal Move!")) {
			JOptionPane.showMessageDialog(this,
					"Illegal Move! Please re-select.", "Error",
					JOptionPane.ERROR_MESSAGE);
			((Piece) obj).unclick();
			boardLocked = false;
		} else if (command.equals("; You Won!")) {
			JOptionPane.showMessageDialog(this, "Congratulations! You Won.");
			boardLocked = true;
		} 
	}

	@Override
	public void actionPerformed(ActionEvent a) {
		if(!boardLocked){
			Piece piece = (Piece) a.getSource();
			if(piece.isClicked())
				return;
			
			piece.setPieceType(PieceType.CIRCLE);
			piece.click();
			
			boardLocked = true;
			String move = "";
			// we add 1 to the row and column of piece to adjust to 1-indexing (vs 0-indexing)
			char outX = Util.intToChar(Util.charToInt(piece.getPosition().getX())+1);
			char outY = Util.intToChar(Util.charToInt(piece.getPosition().getY())+1);
			move += outX;
			move += outY;
			out.print(move);
			try {
				executeCommand(in.readLine(), piece);
				executeCommand(in.readLine(), piece);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
