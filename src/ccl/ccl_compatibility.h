// Copyright 2009-2013 Toby Schneider (https://launchpad.net/~tes)
//                     Massachusetts Institute of Technology (2007-)
//                     Woods Hole Oceanographic Institution (2007-)
//                     DCCL Developers Team (https://launchpad.net/~dccl-dev)
// 
//
// This file is part of the Dynamic Compact Control Language Library
// ("DCCL").
//
// DCCL is free software: you can redistribute them and/or modify
// them under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DCCL is distributed in the hope that they will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DCCL.  If not, see <http://www.gnu.org/licenses/>.


#ifndef DCCLCCLCOMPATIBILITY20120426H
#define DCCLCCLCOMPATIBILITY20120426H

#include "dccl/field_codec_default.h"
#include "dccl/ccl/protobuf/ccl.pb.h"
#include "dccl/ccl/protobuf/ccl_extensions.pb.h"

extern "C"
{
    void dccl3_load(dccl::Codec* dccl);
}



namespace dccl
{
    const unsigned char DCCL_CCL_HEADER = 32;

    class LegacyCCLIdentifierCodec : public DefaultIdentifierCodec
    {
      private:
        dccl::Bitset encode()
        { return encode(0); }
            
        dccl::Bitset encode(const dccl::uint32& wire_value)
        {
            if((wire_value & 0xFFFF0000) == CCL_DCCL_ID_PREFIX)
            {
                // CCL message
                return dccl::Bitset(dccl::BITS_IN_BYTE, wire_value & 0x0000FFFF);
            }
            else
            {
                // DCCL message
                return dccl::DefaultIdentifierCodec::encode(wire_value).prepend(
                    dccl::Bitset(dccl::BITS_IN_BYTE, dccl::DCCL_CCL_HEADER));
            }
                
        }
                
        dccl::uint32 decode(dccl::Bitset* bits)
        {
            unsigned ccl_id = bits->to_ulong();
                
            if(ccl_id == dccl::DCCL_CCL_HEADER)
            {
                // DCCL message
                bits->get_more_bits(dccl::DefaultIdentifierCodec::min_size());
                (*bits) >>= dccl::BITS_IN_BYTE;
                return dccl::DefaultIdentifierCodec::decode(bits);
            }
            else
            {
                // CCL message
                return CCL_DCCL_ID_PREFIX + ccl_id;
            }
        }
            
        unsigned size()
        { return size(0); }
            
        unsigned size(const dccl::uint32& field_value)
        {
            if((field_value & 0xFFFF0000) == CCL_DCCL_ID_PREFIX)
            {
                // CCL message
                return dccl::BITS_IN_BYTE;
            }
            else
            {
                return dccl::BITS_IN_BYTE +
                    dccl::DefaultIdentifierCodec::size(field_value);
            }
        }
            
        unsigned max_size()
        { return dccl::BITS_IN_BYTE + dccl::DefaultIdentifierCodec::max_size(); }

        unsigned min_size()
        { return dccl::BITS_IN_BYTE; }

        // prefixes (dccl.msg).id to indicate that this DCCL
        // message is an encoding of a legacy CCL message
        enum { CCL_DCCL_ID_PREFIX = 0x0CC10000 };
    };

    class LegacyCCLLatLonCompressedCodec : public dccl::TypedFixedFieldCodec<double>
    {
      private:
        dccl::Bitset encode();
        dccl::Bitset encode(const double& wire_value);
        double decode(dccl::Bitset* bits);
        unsigned size();
        enum { LATLON_COMPRESSED_BYTE_SIZE = 3 };            
    };

    class LegacyCCLFixAgeCodec : public dccl::DefaultNumericFieldCodec<dccl::uint32>
    {
      private:
        dccl::Bitset encode()
        {
            return encode(max());
        }
            
        dccl::Bitset encode(const dccl::uint32& wire_value)
        {
            return DefaultNumericFieldCodec<dccl::uint32>::encode(
                std::min<unsigned char>(max(), wire_value / SCALE_FACTOR));
        }
            
        dccl::uint32 decode(dccl::Bitset* bits)
        {
            return SCALE_FACTOR *
                DefaultNumericFieldCodec<dccl::uint32>::decode(bits);
        }
                        
        double max() { return (1 << dccl::BITS_IN_BYTE) - 1; }
        double min() { return 0; }
        void validate() { }
            
        enum { SCALE_FACTOR = 4 };
            
    };
        
            
    class LegacyCCLTimeDateCodec : public dccl::TypedFixedFieldCodec<dccl::uint64>
    {
      public:
        static dccl::uint64 to_uint64_time(const boost::posix_time::ptime& time_date);

      private:
        dccl::Bitset encode();
        dccl::Bitset encode(const dccl::uint64& wire_value);
        dccl::uint64 decode(dccl::Bitset* bits);
        unsigned size();

        enum { MICROSECONDS_IN_SECOND = 1000000 };
        enum { TIME_DATE_COMPRESSED_BYTE_SIZE = 3 };

            
    };

    class LegacyCCLHeadingCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };


    class LegacyCCLHiResAltitudeCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size()
        {
            return HI_RES_ALTITUDE_COMPRESSED_BYTE_SIZE*dccl::BITS_IN_BYTE;
        }
        enum { HI_RES_ALTITUDE_COMPRESSED_BYTE_SIZE = 2 };

    };

        
    class LegacyCCLDepthCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size()
        {
            return FieldCodecBase::dccl_field_options().GetExtension(ccl).bit_size();
        }
            
        void validate()
        {
            FieldCodecBase::require(FieldCodecBase::dccl_field_options().GetExtension(ccl).has_bit_size(), "missing (dccl.field).ccl.bit_size");
        }
            
    };

    class LegacyCCLVelocityCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };

    class LegacyCCLSpeedCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }

        void validate()
        {
            FieldCodecBase::require(FieldCodecBase::dccl_field_options().GetExtension(ccl).has_thrust_mode_tag(), "missing (dccl.field).ccl.thrust_mode_tag");
        }
    };


        
    class LegacyCCLWattsCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };

    class LegacyCCLGFIPitchOilCodec : public dccl::TypedFixedFieldCodec<protobuf::CCLMDATState::GFIPitchOil>
    {
      private:
        dccl::Bitset encode() { return encode(protobuf::CCLMDATState::GFIPitchOil()); }
        dccl::Bitset encode(const protobuf::CCLMDATState::GFIPitchOil& wire_value);
        protobuf::CCLMDATState::GFIPitchOil decode(dccl::Bitset* bits);
        unsigned size() { return GFI_PITCH_OIL_COMPRESSED_BYTE_SIZE*dccl::BITS_IN_BYTE; }
        enum { GFI_PITCH_OIL_COMPRESSED_BYTE_SIZE =2};
                        
    };

    class LegacyCCLSalinityCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };

    class LegacyCCLTemperatureCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };

    class LegacyCCLSoundSpeedCodec : public dccl::TypedFixedFieldCodec<float>
    {
      private:
        dccl::Bitset encode() { return encode(0); }
        dccl::Bitset encode(const float& wire_value);
        float decode(dccl::Bitset* bits);
        unsigned size() { return dccl::BITS_IN_BYTE; }
    };        
        
}

#endif