public class ATM {

    private ATMState currentState;
    private CardReader cardReader;
    private Keypad keypad;
    private Screen screen;
    private CashDispenser cashDispenser;
    private BankDatabase bankDatabase;
    private String currentCardNumber;

    public ATM() {
        cardReader = new CardReader();
        keypad = new Keypad();
        screen = new Screen();
        cashDispenser = new CashDispenser();
        bankDatabase = BankDatabase.getInstance();
        currentState = new InitialState();
    }

    public void setState(ATMState state) {
        this.currentState = state;
    }

    public ATMState getState() {
        return currentState;
    }

    public CardReader getCardReader() {
        return cardReader;
    }

    public Keypad getKeypad() {
        return keypad;
    }

    public Screen getScreen() {
        return screen;
    }

    public CashDispenser getCashDispenser() {
        return cashDispenser;
    }

    public BankDatabase getBankDatabase() {
        return bankDatabase;
    }

    public void setCurrentCardNumber(String cardNumber) {
        this.currentCardNumber = cardNumber;
    }

    public String getCurrentCardNumber() {
        return currentCardNumber;
    }

    public void start() {
        currentState.handle(this);
    }
}
