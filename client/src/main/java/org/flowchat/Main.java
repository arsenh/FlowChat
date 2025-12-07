package org.flowchat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        System.out.println("*** FlowChat Client ***");
        final short port = 8081;
        final String host = "localhost";

        try (Socket socket = new Socket(host, port);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
             Scanner scanner = new Scanner(System.in)) {

            System.out.println("Connected to server.");

            while (true) {
                System.out.print("> "); // prompt
                String input = scanner.nextLine();

                out.println(input);

                String response = in.readLine();
                if (response == null) {
                    System.out.println("Server closed connection.");
                    break;
                }

                System.out.println(response);
            }

        } catch (IOException e) {
            System.err.println("Could not connect to server.");
            System.exit(-1);
        }
    }
}
