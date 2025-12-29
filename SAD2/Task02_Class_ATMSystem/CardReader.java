import java.util.Scanner;

public class CardReader {

    private Scanner scanner = new Scanner(System.in);

    public ATMCard readCard() {
        System.out.print("Enter card number: ");
        String cardNumber = scanner.nextLine();
        if (cardNumber.isEmpty()) return null;
        return new ATMCard(cardNumber);
    }
}
