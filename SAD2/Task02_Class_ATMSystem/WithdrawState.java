public class WithdrawState implements ATMState {

    @Override
    public void handle(ATM atm) {
        atm.getScreen().displayMessage("Enter amount:");
        int amount = atm.getKeypad().getInput();

        boolean success = atm.getBankDatabase()
                .withdraw(atm.getCurrentCardNumber(), amount);

        if (success) {
            atm.getCashDispenser().dispenseCash(amount);

            double remainingBalance = atm.getBankDatabase()
                    .getBalance(atm.getCurrentCardNumber());

            atm.getScreen().displayMessage(
                "تم سحب " + amount + " والرصيد المتبقي هو " + remainingBalance
            );
        } else {
            atm.getScreen().displayMessage("Insufficient balance");
        }
    }
}
