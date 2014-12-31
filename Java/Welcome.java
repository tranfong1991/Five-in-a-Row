import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;
import java.net.UnknownHostException;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

public class Welcome extends JFrame implements ActionListener {	
	private JTextField txtPort = new JTextField();
	private JTextField txtHostIP = new JTextField();
	private JPasswordField txtPassword = new JPasswordField();
	private JButton btnConnect = new JButton("Connect");
	
	private Socket socket;
	private BufferedReader in;
	private PrintStream out;

	public Welcome() {
		super("Welcome To Five in a Row");
		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setLocation(300, 300);
		setSize(300, 150);
		setResizable(false);

		JPanel pnlUpper = new JPanel();
		pnlUpper.setLayout(new GridLayout(3, 2));
		
		pnlUpper.add(new JLabel("Host Name / IP Address"));
		pnlUpper.add(txtHostIP);

		pnlUpper.add(new JLabel("Port Number"));
		pnlUpper.add(txtPort);
		
		pnlUpper.add(new JLabel("Password"));
		pnlUpper.add(txtPassword);

		JPanel pnlLower = new JPanel();

		btnConnect.addActionListener(this);
		pnlLower.add(btnConnect);

		add(pnlUpper, BorderLayout.NORTH);
		add(pnlLower, BorderLayout.SOUTH);
	}

	private void actionErrorUnknown(){
		JOptionPane.showMessageDialog(this, "Unknown Error! Please re-enter.", "Error", 
				JOptionPane.ERROR_MESSAGE);
		this.txtHostIP.setText("");
		this.txtPassword.setText("");
		this.txtPort.setText("");
	}
	
	private void actionErrorNullInput() {
		JOptionPane.showMessageDialog(this, "Empty Input! Please re-enter.",
				"Error", JOptionPane.ERROR_MESSAGE);
	}

	private void actionErrorIncorrectPort(){
		JOptionPane.showMessageDialog(this, "Incorrect Port Number! Please re-enter", "Error", 
				JOptionPane.ERROR_MESSAGE);
	}
	
	private void actionErrorIncorrectHostIP() {
		JOptionPane.showMessageDialog(this,
				"Incorrect Host Name / IP Address! Please re-enter", "Error",
				JOptionPane.ERROR_MESSAGE);
		this.txtHostIP.setText("");
	}
	
	private void actionErrorIncorrectPassword() {
		JOptionPane.showMessageDialog(this,
				"Incorrect Password! Please re-enter.", "Error",
				JOptionPane.ERROR_MESSAGE);
		this.txtPassword.setText("");
	}

	private void actionSuccessConnection() throws IOException {
		this.dispose();
		new Game(socket, in, out);
	}

	/**
	 * -1 = unknown error,
	 * 0 = null input,
	 * 1 = incorrect port number,
	 * 2 = incorrect host name / IP address,
	 * 3 = incorrect password,
	 * 4 = successful connection
	 */
	private int checkInput(){
		if (txtPort.getText().isEmpty() 
			&& txtHostIP.getText().isEmpty()
			&& txtPassword.getPassword()==null)
			return 0;
		
		try {
			if(this.socket == null){
				this.socket = new Socket(txtHostIP.getText(), Integer.parseInt(txtPort.getText()));
				this.in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
				this.out = new PrintStream(socket.getOutputStream(), true);
			}

			System.out.println(in.readLine());
			out.print(new String(txtPassword.getPassword()));
			String response = in.readLine();
			System.out.println(response);
			if(response.equals("; Incorrect Password!"))	
				return 3;
		} catch (NumberFormatException e) {
			return 1;
		} catch (UnknownHostException e) {
			return 2;
		} catch (IOException e) {
			return -1;
		}
		
		return 4;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		int state = checkInput();

		switch (state) {
		case -1:
			actionErrorUnknown();
			break;
		case 0:
			actionErrorNullInput();
			break;
		case 1:
			actionErrorIncorrectPort();
			break;
		case 2:
			actionErrorIncorrectHostIP();
			break;
		case 3:
			actionErrorIncorrectPassword();
			break;
		default:
			try {
				actionSuccessConnection();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			break;
		}
	}
}