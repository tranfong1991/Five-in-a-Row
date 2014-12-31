import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintStream;
import java.net.Socket;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

enum Mode {
	EASY, MEDIUM, HARD;

	static Mode toMode(String s) {
		if (s.equals("EASY"))
			return EASY;
		if (s.equals("MEDIUM"))
			return MEDIUM;
		return HARD;
	}
}

/*
 * Game window
 * Lets player play against AI with different difficulties
 * Also lets player choose AI vs. AI
 */
public class Game extends JFrame implements ActionListener {
	private final String[] MODES = { "EASY", "MEDIUM", "HARD" };
	private static final String ABOUT = "Five in a Row. v.1";

	private Board board;
	private Mode currentMode;

	private JTextField txtHostIP = new JTextField();
	private JTextField txtPort = new JTextField();
	private JPasswordField txtPassword = new JPasswordField();
	private final JComponent[] inputs = new JComponent[] {
			new JLabel("Host Name / IP Address"), txtHostIP,
			new JLabel("Port Number"), txtPort, new JLabel("Password"),
			txtPassword };

	private JMenuBar menuBar = new JMenuBar();
	private JMenu mnuGame = new JMenu("Game");
	private JMenu mnuAbout = new JMenu("About");
	private JMenuItem mniNewGame = new JMenuItem("New Game");
	private JMenuItem mniSetMode = new JMenuItem("Set Mode");
	private JMenuItem mniSetAIAI = new JMenuItem("Set AI vs. AI");
	private JMenuItem mniUndo = new JMenuItem("Undo");
	private JMenuItem mniExit = new JMenuItem("Exit");
	private JMenuItem mniAbout = new JMenuItem("About");

	private Socket socket;
	private BufferedReader in;
	private PrintStream out;

	public Game(Socket socket, BufferedReader in, PrintStream out)
			throws IOException {
		super("Five In A Row");
		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setLocation(300, 300);
		setSize(500, 500);
		setResizable(false);

		//Default mode
		this.currentMode = Mode.EASY;

		this.socket = socket;
		this.in = in;
		this.out = out;

		this.board = new Board(in, out);

		mniNewGame.addActionListener(this);
		mnuGame.add(mniNewGame);

		mniSetMode.addActionListener(this);
		mnuGame.add(mniSetMode);

		mniSetAIAI.addActionListener(this);
		mnuGame.add(mniSetAIAI);

		mniUndo.addActionListener(this);
		mnuGame.add(mniUndo);

		mniExit.addActionListener(this);
		mnuGame.add(mniExit);

		mniAbout.addActionListener(this);
		mnuAbout.add(mniAbout);

		menuBar.add(mnuGame);
		menuBar.add(mnuAbout);

		setJMenuBar(menuBar);
		add(board);
	}

	private void actionNewGame() {
		board.reset();
		board.setLock(false);
		out.print("RESET");
	}

	private void actionSetMode() {
		String choice = (String) JOptionPane.showInputDialog(this,
				"Please choose a mode:", "Set Mode", JOptionPane.PLAIN_MESSAGE,
				null, MODES, this.currentMode.toString());

		if (choice != null) {
			this.currentMode = Mode.toMode(choice);
			out.print(currentMode.toString());
			actionNewGame();
		}
	}
	private void actionSetAIAI() {
		board.reset();
		JOptionPane.showMessageDialog(this, inputs, "AI vs. AI Information",
				JOptionPane.PLAIN_MESSAGE);
		if (txtHostIP.getText().isEmpty() || txtPort.getText().isEmpty()
				|| txtPassword.getPassword() == null)
			JOptionPane.showMessageDialog(this, "Please enter all field",
					"Error", JOptionPane.ERROR_MESSAGE);
		else {
			out.print("AI-AI " + txtHostIP.getText() + " " + txtPort.getText()
					+ " " + new String(txtPassword.getPassword()));

			String command = "";

			try {
				command = in.readLine();
			} catch (IOException e) {
				e.printStackTrace();
			}

			if(command.equals("; Incorrect Password!"))	{//Incorrect Password
				JOptionPane.showMessageDialog(this,
						"Incorrect Password! Please re-enter", "Error",
						JOptionPane.ERROR_MESSAGE);
				txtPassword.setText("");
			} else if(command.equals("; NO SUCH HOST"))	{// No host found
				JOptionPane.showMessageDialog(this,
						"Host not found! Please re-enter", "Error",
						JOptionPane.ERROR_MESSAGE);
				txtHostIP.setText("");
			} else
				startAIAI();
		}
	}

	//Continuously reads command from the server and executes based on the command
	private void startAIAI() {
		int status;
		String command = "";

		board.setLock(true);
		board.setToggle(true);
		do {
			try {
				command = in.readLine();
			} catch (IOException e) {
				e.printStackTrace();
			}
			status = board.executeAIAI(command);
		} while (status > 0);
	}

	private void actionUndo() {
		if(board.isLocked())
			return;
		
		out.print("UNDO");
		try {
			String okMessage = in.readLine();
			String playerUndo = in.readLine();
			if(okMessage.equals("OK")) {
				System.out.println(okMessage);
				if(playerUndo.startsWith("; UNDO")){
					String aiUndo = in.readLine();
					int x1 = Util.charToInt(playerUndo.charAt(7));
					int y1 = Util.charToInt(playerUndo.charAt(8));
					int x2 = Util.charToInt(aiUndo.charAt(7));
					int y2 = Util.charToInt(aiUndo.charAt(8));
				
					board.unclick(x1, y1);
					board.unclick(x2, y2);
				}
				else
					JOptionPane.showMessageDialog(this, "No more move. Max = 7");
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void actionExit() {
		try {
			out.print("EXIT");
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			System.exit(0);
		}
	}

	private void actionAbout() {
		JOptionPane.showMessageDialog(this, ABOUT, "About",
				JOptionPane.INFORMATION_MESSAGE);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
		if (source == mniNewGame)
			actionNewGame();
		else if (source == mniSetMode)
			actionSetMode();
		else if (source == mniSetAIAI)
			actionSetAIAI();
		else if (source == mniUndo)
			actionUndo();
		else if (source == mniExit)
			actionExit();
		else
			actionAbout();
	}
}
