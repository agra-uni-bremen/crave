#define BOOST_TEST_MODULE allsat
#include <boost/test/unit_test.hpp>

#include <platzhalter/ConstrainedRandom.hpp>
#include <platzhalter/AllSAT.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

//using namespace std;
using namespace platzhalter;

using boost::format;

struct Context_Fixture {
  Context_Fixture ()
  {
  }
  protected:
  Generator<AllSAT> gen;
};


/***
* SIF peripheral is a serial I/O device 
* SIF peripheral has three interfaces
****- Bus Interface
****- TX Interface
****- RX Interface
* SIF is configured via the BUS interface
* TX and RX paths can be configured independently
* SIF cannot support bursts
***/
class sif_seq_item : public rand_obj_of<AllSAT> {
public:
    sif_seq_item() : 
        //doPolling(this), 
        //transmit_enable(this), 
        //receive_enable(this),
        //loop_back(this),
        //echo_mode(this),
        //char_length(this),
        //tx_stop_bit2(this),
        tx_enable_parity(this),
        tx_odd_parity(this),
        //tx_heading(this),
        //tx_inversion(this),
        //rx_stop_bit2(this),
        //rx_enable_parity(this),
        //rx_odd_parity(this),
        //rx_heading(this),
        //rx_inversion(this),
        //tx_chars(this),
        //rx_chars(this),
        //tx_clk_rate(this),
        //rx_clk_rate(this)
        dummy()
    {
      //char_length.range(2, 8);
      //tx_clk_rate.range(1, 16);
      //rx_clk_rate.range(1, 16);

      //constraint("tx_parity", !tx_odd_parity() || tx_enable_parity());
      //constraint("rx_parity", !rx_odd_parity() || rx_enable_parity());
      //constraint( !rx_odd_parity() || rx_enable_parity());
      constraint( !tx_odd_parity() || tx_enable_parity());
    }

    friend ostream& operator<<(ostream& os, const sif_seq_item& ssi) {
        ///os << "BeginConstraint" << std::endl; 
        ///os << "doPolling = " << ssi.doPolling << std::endl; 
        ///os << "transmit_enable = " << ssi.transmit_enable << std::endl; 
        ///os << "receive_enable = " << ssi.receive_enable << std::endl; 
        ///os << "loop_back = " << ssi.loop_back << std::endl; 
        ///os << "echo_mode = " << ssi.echo_mode << std::endl; 
        ///os << "char_length = " << (uint) ssi.char_length << std::endl; 
        ///os << "tx_stop_bit2 = " << ssi.tx_stop_bit2 << std::endl; 
        ///os << "tx_enable_parity = " << ssi.tx_enable_parity << std::endl; 
        ///os << "tx_odd_parity = " << ssi.tx_odd_parity << std::endl; 
        ///os << "tx_heading = " << ssi.tx_heading << std::endl; 
        ///os << "tx_inversion = " << ssi.tx_inversion << std::endl;
        ///os << "rx_stop_bit2 = " << ssi.rx_stop_bit2 << std::endl;
        ///os << "rx_enable_parity = " << ssi.rx_enable_parity << std::endl;
        ///os << "rx_odd_parity = " << ssi.rx_odd_parity << std::endl;
        ///os << "rx_heading = " << ssi.rx_heading << std::endl; 
        ///os << "rx_inversion = " << ssi.rx_inversion << std::endl;
        ///os << "tx_chars = " << ssi.tx_chars << std::endl;
        ///os << "rx_chars = " << ssi.rx_chars << std::endl;
        ///os << "tx_clk_rate = " << ssi.tx_clk_rate << std::endl;
        ///os << "rx_clk_rate = " << ssi.rx_clk_rate << std::endl;
        ///os << "TxCharVec = "; 
        ///for (uint i = 0; i < ssi.TxCharVec.size(); i++) {
        ///  os << "#" << (int) ssi.TxCharVec[i] << " ";
        ///}
        ///os << std::endl; 
        ///os << "RxCharVec = "; 
        ///for (uint i = 0; i < ssi.RxCharVec.size(); i++) {
        ///  os << "#" << (int) ssi.RxCharVec[i] << " ";
        ///}
        ///os << std::endl; 
        os << "CHECK: tx_odd_parity / enable  = " << ssi.tx_odd_parity
           << " " << ssi.tx_enable_parity << std::endl; 
        assert( !ssi.tx_odd_parity || ssi.tx_enable_parity ); 

        os << "EndConstraint" << std::endl; 
        return os; 
    }

    //// CTRL
    ///// Determines wether sequences shall use interrupt-based or polling based communication with DUT
    //randv<bool> doPolling;
    ///// Enable the TX Path of DUT, if disabled DUT will not send anything
    //randv<bool> transmit_enable;
    ///// Enable the RX Path of DUT, if disabled DUT will not receive anything
    //randv<bool> receive_enable;
    //// MODE_CFG
    ///// if enabled DUT will loop TX Data back as RX data, NOT Implemented in DUT yet
    //randv<bool> loop_back;
    ///// if enabled DUT will send back characters immediately on receiving them, NOT Implemented in DUT yet
    //randv<bool> echo_mode;
    //// PRTC_CFG
    ///// Number of bits of one character for RX and TX, in UARTS it is commonly set to 8
    //randv<uint8_t> char_length;
    ///// Add a second stop bit to TX characters
    //randv<bool> tx_stop_bit2;
    ///// Enable parity calculation on TX
    randv<bool> tx_enable_parity;
    ///// Set whether even or odd parity is used
    randv<bool> tx_odd_parity;
    ///// bit reversal of the characters being sent
    //randv<bool> tx_heading;
    ///// invert all bits of TX characters
    //randv<bool> tx_inversion;
    ///// expect second stop_bit on RX characters
    //randv<bool> rx_stop_bit2;
    ///// enable parity checking for RX characters
    //randv<bool> rx_enable_parity;
    ///// set even/odd parity mode for parity checking
    //randv<bool> rx_odd_parity;
    ///// bit reversal of RX characters
    //randv<bool> rx_heading;
    ///// bit inversion of RX characters
    //randv<bool> rx_inversion;

    ///// These vectors are used as containers for storing TX or RX data chars
    //std::vector<char> TxCharVec;
    //std::vector<char> RxCharVec;

    ///// how many chars shall be sent
    //randv<int> tx_chars;
    ///// how many chars shall be received
    //randv<int> rx_chars;

    ///// Sets the Clock dividers for the serial interface, i.e. the frequency for sending/receiving bits
    //randv<int> tx_clk_rate;
    //randv<int> rx_clk_rate;
    int dummy;
};

BOOST_FIXTURE_TEST_SUITE(allsat, Context_Fixture )

BOOST_AUTO_TEST_CASE ( gen10k )
{
  Variable<bool> r1, r2;
  gen( !r1 || r2);

  for (int i=0;i<10000; i++) {
    gen();
    BOOST_REQUIRE( !gen[r1] || gen[r2] );
  }
}


BOOST_AUTO_TEST_CASE ( randv10k )
{
  sif_seq_item it;

  for (int i=0;i<10000; i++) {
    BOOST_REQUIRE(it.next());
    std::cout << it << std::endl;
    BOOST_REQUIRE( !it.tx_odd_parity ||  it.tx_enable_parity );
    //BOOST_REQUIRE( !it.rx_odd_parity ||  it.rx_enable_parity );
  }
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
