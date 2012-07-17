
#include "goby/acomms/amac.h"
#include "goby/acomms/connect.h"
#include "goby/common/logger.h"

#include <iostream>

using goby::acomms::operator<<;

void init_transmission(const goby::acomms::protobuf::ModemTransmission& msg);

int main(int argc, char* argv[])
{
    goby::glog.set_name(argv[0]);
    goby::glog.add_stream(goby::common::logger::DEBUG1, &std::clog);

    //
    // 1. Create a MACManager
    //
    goby::acomms::MACManager mac;

    //
    // 2. Configure with a few slots
    //
    goby::acomms::protobuf::MACConfig cfg;    
    cfg.set_modem_id(1);
    cfg.set_type(goby::acomms::protobuf::MAC_FIXED_DECENTRALIZED);
    goby::acomms::protobuf::ModemTransmission* slot = cfg.add_slot();
    slot->set_src(1);
    slot->set_rate(0);
    slot->set_type(goby::acomms::protobuf::ModemTransmission::DATA);
    slot->set_slot_seconds(5);
    
    
    //
    // 3. Set up the callback
    //

    // give a callback to use for actually initiating the transmission. this would be bound to goby::acomms::ModemDriverBase::initiate_transmission if using libmodemdriver.
    goby::acomms::connect(&mac.signal_initiate_transmission, &init_transmission);

    //
    // 4. Let it run for a bit alone in the world
    //
    mac.startup(cfg);
    for(unsigned i = 1; i < 150; ++i)
    {
        mac.do_work();
        usleep(100000);
    }
    
    //
    // 5. Add some slots (modem ids 2 & 3, and LBL ping from 1): remember MACManager is a std::list<goby::acomms::protobuf::ModemTransmission> at heart
    //

    // one way we can do this
    goby::acomms::protobuf::ModemTransmission new_slot;
    new_slot.set_src(2);
    new_slot.set_rate(0);
    new_slot.set_type(goby::acomms::protobuf::ModemTransmission::DATA);
    new_slot.set_slot_seconds(5);
    
    mac.push_back(new_slot);

    // another way
    mac.resize(mac.size()+1);
    mac.back().CopyFrom(mac.front());
    mac.back().set_src(3);

    mac.resize(mac.size()+1);
    mac.back().CopyFrom(mac.front());
    mac.back().set_type(goby::acomms::protobuf::ModemTransmission::DRIVER_SPECIFIC);
    mac.back().SetExtension(micromodem::protobuf::type,
                            micromodem::protobuf::MICROMODEM_REMUS_LBL_RANGING);

    // must call update after manipulating MACManager before calling do_work again.
    mac.update();
    
    
    //
    // 6. Run it
    //
    
    for(;;)
    {
        mac.do_work();
        usleep(100000);
    }
    
    return 0;
}

void init_transmission(const goby::acomms::protobuf::ModemTransmission& msg)
{
    std::cout << "starting transmission with these values: " << msg << std::endl;
}