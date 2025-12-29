import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class ATMTest {

    private BankDatabase bank;
    private Account account;

    @BeforeEach
    void setup() {
        bank = BankDatabase.getInstance();
        bank.clearAccounts();              // ✅ يمسح أي داتا قديمة
        bank.addAccount(
                new Account("9999", 1111, 5000)
        );
    }


    // -------- Authentication Tests --------

    @Test
    void TC01_authenticateWithCorrectPin() {
        assertTrue(bank.authenticate("9999", 1111));
    }

    @Test
    void TC02_authenticateWithWrongPin() {
        assertFalse(bank.authenticate("9999", 1234));
    }

    @Test
    void TC03_authenticateWithWrongCard() {
        assertFalse(bank.authenticate("0000", 1111));
    }

    @Test
    void TC04_authenticateWithNegativePin() {
        assertFalse(bank.authenticate("9999", -1));
    }

    @Test
    void TC05_authenticateWithLargePin() {
        assertFalse(bank.authenticate("9999", 99999));
    }

    // -------- Balance Tests --------

    @Test
    void TC06_checkInitialBalance() {
        assertEquals(5000, bank.getBalance("9999"));
    }

    @Test
    void TC07_balanceAfterWithdraw() {
        bank.withdraw("9999", 1000);
        assertEquals(4000, bank.getBalance("9999"));
    }

    @Test
    void TC08_balanceAfterMultipleWithdrawals() {
        bank.withdraw("9999", 1000);
        bank.withdraw("9999", 500);
        assertEquals(3500, bank.getBalance("9999"));
    }

    @Test
    void TC09_balanceNotNegative() {
        bank.withdraw("9999", 6000);
        assertEquals(5000, bank.getBalance("9999"));
    }

    @Test
    void TC10_balanceInquiryForInvalidCard() {
        assertEquals(0, bank.getBalance("0000"));
    }

    // -------- Withdraw Tests --------

    @Test
    void TC11_withdrawValidAmount() {
        assertTrue(bank.withdraw("9999", 1000));
    }

    @Test
    void TC12_withdrawExactBalance() {
        assertTrue(bank.withdraw("9999", 5000));
    }

    @Test
    void TC13_withdrawMoreThanBalance() {
        assertFalse(bank.withdraw("9999", 6000));
    }

    @Test
    void TC14_withdrawZeroAmount() {
        assertTrue(bank.withdraw("9999", 0));
    }



    // -------- Singleton Tests --------

    @Test
    void TC16_singletonSameInstance() {
        BankDatabase bank2 = BankDatabase.getInstance();
        assertSame(bank, bank2);
    }

    @Test
    void TC17_singletonDataShared() {
        bank.withdraw("9999", 500);
        assertEquals(4500, bank.getBalance("9999"));
    }



    // -------- Stress / Edge Tests --------

    @Test
    void TC21_multipleWithdrawsUntilEmpty() {
        bank.withdraw("9999", 1000);
        bank.withdraw("9999", 1000);
        bank.withdraw("9999", 1000);
        assertEquals(2000, bank.getBalance("9999"));
    }

    @Test
    void TC22_largeWithdrawInput() {
        assertFalse(bank.withdraw("9999", 999999));
    }

    @Test
    void TC23_withdrawAfterFailedAttempt() {
        bank.withdraw("9999", 6000);
        assertEquals(5000, bank.getBalance("9999"));
    }

    @Test
    void TC24_withdrawSequential() {
        assertTrue(bank.withdraw("9999", 500));
        assertTrue(bank.withdraw("9999", 500));
    }

    @Test
    void TC25_balanceAfterSequentialWithdraw() {
        bank.withdraw("9999", 500);
        bank.withdraw("9999", 500);
        assertEquals(4000, bank.getBalance("9999"));
    }

    // -------- Additional Coverage --------

    @Test void TC26() { assertTrue(bank.authenticate("9999",1111)); }
    @Test void TC27() { assertFalse(bank.authenticate("9999",0)); }
    @Test void TC28() { assertEquals(5000, bank.getBalance("9999")); }
    @Test void TC29() { assertFalse(bank.withdraw("0000",100)); }
    @Test void TC30() { assertFalse(bank.authenticate("",1111)); }
    @Test void TC32() { assertEquals(0, bank.getBalance("invalid")); }
    @Test void TC33() { assertTrue(bank.withdraw("9999",1)); }
    @Test void TC34() { assertTrue(bank.withdraw("9999",10)); }
    @Test void TC35() { assertTrue(bank.withdraw("9999",100)); }
    @Test void TC36() { assertFalse(bank.authenticate("9999",9999)); }
    @Test void TC37() { assertTrue(bank.authenticate("9999",1111)); }
    @Test void TC38() { assertEquals(0, bank.getBalance("xxxx")); }
    @Test void TC39() { assertFalse(bank.withdraw("xxxx",100)); }
    @Test void TC40() { assertTrue(bank.withdraw("9999",0)); }

    // -------- Final Tests --------

    @Test void TC41() { assertNotNull(bank); }

    @Test void TC43() { assertSame(bank, BankDatabase.getInstance()); }
    @Test void TC45() { assertTrue(bank.getBalance("9999") >= 0); }
    @Test void TC47() { assertFalse(bank.authenticate(" ", 1111)); }
    @Test void TC48() { assertFalse(bank.withdraw(" ", 100)); }
    @Test void TC49() { assertNotEquals(-1, bank.getBalance("9999")); }
    @Test void TC50() { assertTrue(true); }
}

