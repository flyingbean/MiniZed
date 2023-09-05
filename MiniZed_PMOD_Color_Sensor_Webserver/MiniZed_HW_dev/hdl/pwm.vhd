----------------------------------------------------------------------------------
--Tool Version: Vivado v.2016.4  
--Date        : Fri Nov 15 12:26:49 2019
--Host        :  
--Command     :  
--Design      : PWM Logic
--Purpose     :    
----------------------------------------------------------------------------------


LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;

ENTITY pwm IS
  GENERIC(
      sys_clk         : INTEGER := 50_000_000;  --system clock frequency in Hz
      pwm_freq        : INTEGER := 1_000;      --PWM switching frequency in Hz
      bits_resolution : INTEGER := 4;          --bits of resolution setting the duty cycle
      phases          : INTEGER := 1);         --number of output pwms and phases
  PORT(
      clk       : IN  STD_LOGIC;                                    --system clock
      reset_n   : IN  STD_LOGIC;                                    --asynchronous reset
      ena       : IN  STD_LOGIC;                                    --latches in new duty cycle
      duty      : IN  STD_LOGIC_VECTOR(bits_resolution-1 DOWNTO 0); --duty cycle
      pwm_out   : OUT STD_LOGIC_VECTOR(phases-1 DOWNTO 0);          --pwm outputs
      pwm_n_out : OUT STD_LOGIC_VECTOR(phases-1 DOWNTO 0));         --pwm inverse outputs
END pwm;

ARCHITECTURE LOGIC OF pwm IS
  CONSTANT  period     :  INTEGER := sys_clk/pwm_freq;                      --number of clocks in one pwm period
  TYPE counters IS ARRAY (0 TO phases-1) OF INTEGER RANGE 0 TO period - 1;  --data type for array of period counters
  SIGNAL  count        :  counters := (OTHERS => 0);                        --array of period counters
  SIGNAL   half_duty_new  :  INTEGER RANGE 0 TO period/2 := 0;              --number of clocks in 1/2 duty cycle
  TYPE half_duties IS ARRAY (0 TO phases-1) OF INTEGER RANGE 0 TO period/2; --data type for array of half duty values
  SIGNAL  half_duty    :  half_duties := (OTHERS => 0);                     --array of half duty values (for each phase)
BEGIN
  PROCESS(clk, reset_n)
  BEGIN
    IF(reset_n = '0') THEN    
      count <= (OTHERS => 0);                                                 
      pwm_out <= (OTHERS => '0');                                             
      pwm_n_out <= (OTHERS => '0');                                           
    ELSIF(clk'EVENT AND clk = '1') THEN                                       
      IF(ena = '1') THEN                                                      
        half_duty_new <= conv_integer(duty)*period/(2**bits_resolution)/2;    
      END IF;
      FOR i IN 0 to phases-1 LOOP                                             
        IF(count(0) = period - 1 - i*period/phases) THEN                       
          count(i) <= 0;                                                          
          half_duty(i) <= half_duty_new;                                          
        ELSE                                                                    
          count(i) <= count(i) + 1;                                               
        END IF;
      END LOOP;
      FOR i IN 0 to phases-1 LOOP    --control outputs for each phase
        IF(count(i) = half_duty(i)) THEN                                        
          pwm_out(i) <= '0';                                                      
          pwm_n_out(i) <= '1';                                                    
        ELSIF(count(i) = period - half_duty(i)) THEN                            
          pwm_out(i) <= '1';                                                      
          pwm_n_out(i) <= '0';                                                    
        END IF;
      END LOOP;
    END IF;
  END PROCESS;
END LOGIC;
