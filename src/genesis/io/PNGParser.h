// ===================================================================================
//  File:    PNGParser.h
//  Project: Genesis Engine
//  Purpose: A lightweight, self-contained, header-only PNG parser.
//           This parser is framework-free and written in pure C++ to read the
//           PNG format, decompress its data, and provide raw pixel access.
//           It supports the most common PNG formats (e.g., 8-bit RGBA).
// ===================================================================================

#pragma once

#include "../common/DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <numeric>
#include <algorithm>

// --- Internal Minimalist DEFLATE (zlib) Decompressor ---
// To adhere to the "framework-free" principle, we embed a minimalist, well-known
// public domain DEFLATE decompressor directly into this file. This avoids linking
// to external libraries like zlib or libpng.
namespace genesis::detail {
    // A tiny, public domain, header-only zlib/DEFLATE decompressor.
    // Source: tinf by Joergen Ibsen
    #define TINF_HEADER_ONLY
    #include <cstring>
    enum {
        TINF_OK = 0, TINF_DATA_ERROR = -3
    };
    typedef struct {
        const unsigned char *source;
        unsigned int tag;
        unsigned int bitcount;
    } tinf_bitstream;
    typedef struct {
        unsigned short table[16];
        unsigned short symbol[288];
    } tinf_tree;
    static unsigned int tinf_read_bits(tinf_bitstream *bs, int num_bits) {
        unsigned int res = 0;
        int i;
        for (i = 0; i < num_bits; ++i) {
            if (bs->bitcount == 0) {
                bs->tag = *bs->source++;
                bs->bitcount = 8;
            }
            res |= (bs->tag & 1) << i;
            bs->tag >>= 1;
            bs->bitcount--;
        }
        return res;
    }
    static void tinf_build_tree(tinf_tree *tree, const unsigned char *lengths, unsigned int num_syms) {
        unsigned short offs[16];
        unsigned int i, num, len;
        for (i = 0; i < 16; ++i) {
            tree->table[i] = 0;
        }
        for (i = 0; i < num_syms; ++i) {
            tree->table[lengths[i]]++;
        }
        tree->table[0] = 0;
        for (i = 1, num = 0; i < 16; ++i) {
            offs[i] = num;
            num += tree->table[i];
        }
        for (i = 0; i < num_syms; ++i) {
            if ((len = lengths[i]) != 0) {
                tree->symbol[offs[len]++] = i;
            }
        }
    }
    static int tinf_decode_symbol(tinf_bitstream *bs, const tinf_tree *tree) {
        int sum = 0, cur = 0, len = 0;
        do {
            cur = 2*cur + tinf_read_bits(bs, 1);
            len++;
            sum += tree->table[len];
            cur -= tree->table[len];
        } while (cur >= 0);
        return tree->symbol[sum + cur];
    }
    static void tinf_uncompress_block(unsigned char *dest, unsigned int *dest_len, const unsigned char *source, unsigned int source_len) {
        static const unsigned char clcidx[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
        static const unsigned short lndists[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
        static const unsigned char lnexbs[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
        static const unsigned short dndists[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
        static const unsigned char dnexbs[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
        tinf_bitstream bs;
        tinf_tree ltree, dtree;
        unsigned char lengths[288+32];
        unsigned int bfinal, btype, hlit, hdist, hclen, i, j, len, dist, pos = 0;
        bs.source = source;
        bs.bitcount = 0;
        do {
            bfinal = tinf_read_bits(&bs, 1);
            btype = tinf_read_bits(&bs, 2);
            if (btype == 0) {
                tinf_read_bits(&bs, (8 - bs.bitcount) & 7);
                len = bs.source[0] | (bs.source[1] << 8);
                bs.source += 4;
                memcpy(dest + pos, bs.source, len);
                bs.source += len;
                pos += len;
            } else if (btype == 1 || btype == 2) {
                if (btype == 1) {
                    for (i = 0; i < 144; ++i) lengths[i] = 8;
                    for (i = 144; i < 256; ++i) lengths[i] = 9;
                    for (i = 256; i < 280; ++i) lengths[i] = 7;
                    for (i = 280; i < 288; ++i) lengths[i] = 8;
                    tinf_build_tree(&ltree, lengths, 288);
                    for (i = 0; i < 32; ++i) lengths[i] = 5;
                    tinf_build_tree(&dtree, lengths, 32);
                } else {
                    hlit = tinf_read_bits(&bs, 5) + 257;
                    hdist = tinf_read_bits(&bs, 5) + 1;
                    hclen = tinf_read_bits(&bs, 4) + 4;
                    memset(lengths, 0, sizeof(lengths));
                    for (i = 0; i < hclen; ++i) {
                        lengths[clcidx[i]] = tinf_read_bits(&bs, 3);
                    }
                    tinf_build_tree(&ltree, lengths, 19);
                    for (i = 0; i < hlit + hdist;) {
                        int sym = tinf_decode_symbol(&bs, &ltree);
                        if (sym < 16) {
                            lengths[i++] = sym;
                        } else {
                            len = 0;
                            if (sym == 16) {
                                len = tinf_read_bits(&bs, 2) + 3;
                                for (j = 0; j < len; ++j) lengths[i+j] = lengths[i-1];
                                i += len;
                            } else if (sym == 17) {
                                len = tinf_read_bits(&bs, 3) + 3;
                                i += len;
                            } else {
                                len = tinf_read_bits(&bs, 7) + 11;
                                i += len;
                            }
                        }
                    }
                    tinf_build_tree(&ltree, lengths, hlit);
                    tinf_build_tree(&dtree, lengths + hlit, hdist);
                }
                for (;;) {
                    int sym = tinf_decode_symbol(&bs, &ltree);
                    if (sym == 256) break;
                    if (sym < 256) {
                        dest[pos++] = sym;
                    } else {
                        sym -= 257;
                        len = tinf_read_bits(&bs, lnexbs[sym]) + lndists[sym];
                        sym = tinf_decode_symbol(&bs, &dtree);
                        dist = tinf_read_bits(&bs, dnexbs[sym]) + dndists[sym];
                        for (j = 0; j < len; ++j) {
                            dest[pos + j] = dest[pos - dist + j];
                        }
                        pos += len;
                    }
                }
            }
        } while (!bfinal);
        *dest_len = pos;
    }
} // namespace genesis::detail

class PNGParser
{
public:
    explicit PNGParser(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) throw std::runtime_error("PNGParser: Could not open file " + filepath);

        std::vector<uint8> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // 1. Check Signature
        const std::vector<uint8> signature = {137, 80, 78, 71, 13, 10, 26, 10};
        if (buffer.size() < 8 || !std::equal(signature.begin(), signature.end(), buffer.begin())) {
            throw std::runtime_error("PNGParser: Invalid PNG signature.");
        }

        // 2. Parse Chunks
        size_t offset = 8;
        std::vector<uint8> idat_data;
        uint8_t color_type = 0, bit_depth = 0;

        while(offset < buffer.size()) {
            uint32_t length = read_big_endian_32(buffer.data() + offset);
            offset += 4;
            std::string type(buffer.begin() + offset, buffer.begin() + offset + 4);
            offset += 4;

            if (type == "IHDR") {
                m_width = read_big_endian_32(buffer.data() + offset);
                m_height = read_big_endian_32(buffer.data() + offset + 4);
                bit_depth = buffer[offset + 8];
                color_type = buffer[offset + 9];
                if (bit_depth != 8) throw std::runtime_error("PNGParser: Only 8-bit depth supported.");
                if (color_type != 2 && color_type != 6) throw std::runtime_error("PNGParser: Only RGB and RGBA color types supported.");
            } else if (type == "IDAT") {
                idat_data.insert(idat_data.end(), buffer.begin() + offset, buffer.begin() + offset + length);
            } else if (type == "IEND") {
                break;
            }
            offset += length + 4; // Skip data and CRC
        }

        // 3. Decompress IDAT Data
        uint32_t bpp = (color_type == 6) ? 4 : 3;
        unsigned int raw_size = m_width * m_height * bpp + m_height;
        std::vector<uint8> raw_data(raw_size);
        genesis::detail::tinf_uncompress_block(raw_data.data(), &raw_size, idat_data.data(), idat_data.size());
        
        // 4. Unfilter Scanlines
        m_pixelData.resize(m_width * m_height * 4); // We will always output RGBA
        const uint32_t stride = m_width * bpp;
        
        for (uint32_t y = 0; y < m_height; ++y) {
            uint8_t filter_type = raw_data[y * (stride + 1)];
            const uint8_t* scanline = &raw_data[y * (stride + 1) + 1];

            for (uint32_t x = 0; x < m_width; ++x) {
                uint8_t r = 0, g = 0, b = 0, a = 255;
                const uint8_t* current_pixel = scanline + x * bpp;
                
                if (bpp >= 3) {
                    r = current_pixel[0];
                    g = current_pixel[1];
                    b = current_pixel[2];
                }
                if (bpp == 4) {
                    a = current_pixel[3];
                }

                uint8_t recon_r = r, recon_g = g, recon_b = b, recon_a = a;
                const uint32_t out_idx = (y * m_width + x) * 4;

                auto get_prior_pixel = [&](int channel) -> uint8_t {
                    return (x > 0) ? m_pixelData[out_idx - 4 + channel] : 0;
                };
                auto get_above_pixel = [&](int channel) -> uint8_t {
                    return (y > 0) ? m_pixelData[out_idx - m_width * 4 + channel] : 0;
                };
                auto get_above_left_pixel = [&](int channel) -> uint8_t {
                    return (x > 0 && y > 0) ? m_pixelData[out_idx - m_width * 4 - 4 + channel] : 0;
                };
                
                auto paeth_predictor = [](uint8_t a, uint8_t b, uint8_t c) {
                    int p = a + b - c;
                    int pa = std::abs(p - a);
                    int pb = std::abs(p - b);
                    int pc = std::abs(p - c);
                    if (pa <= pb && pa <= pc) return a;
                    if (pb <= pc) return b;
                    return c;
                };

                switch (filter_type) {
                    case 0: break; // None
                    case 1: // Sub
                        recon_r += get_prior_pixel(0);
                        recon_g += get_prior_pixel(1);
                        recon_b += get_prior_pixel(2);
                        recon_a += get_prior_pixel(3);
                        break;
                    case 2: // Up
                        recon_r += get_above_pixel(0);
                        recon_g += get_above_pixel(1);
                        recon_b += get_above_pixel(2);
                        recon_a += get_above_pixel(3);
                        break;
                    case 3: // Average
                        recon_r += (get_prior_pixel(0) + get_above_pixel(0)) / 2;
                        recon_g += (get_prior_pixel(1) + get_above_pixel(1)) / 2;
                        recon_b += (get_prior_pixel(2) + get_above_pixel(2)) / 2;
                        recon_a += (get_prior_pixel(3) + get_above_pixel(3)) / 2;
                        break;
                    case 4: // Paeth
                        recon_r += paeth_predictor(get_prior_pixel(0), get_above_pixel(0), get_above_left_pixel(0));
                        recon_g += paeth_predictor(get_prior_pixel(1), get_above_pixel(1), get_above_left_pixel(1));
                        recon_b += paeth_predictor(get_prior_pixel(2), get_above_pixel(2), get_above_left_pixel(2));
                        recon_a += paeth_predictor(get_prior_pixel(3), get_above_pixel(3), get_above_left_pixel(3));
                        break;
                    default: throw std::runtime_error("PNGParser: Unknown filter type.");
                }

                m_pixelData[out_idx + 0] = recon_r;
                m_pixelData[out_idx + 1] = recon_g;
                m_pixelData[out_idx + 2] = recon_b;
                if (bpp == 4) m_pixelData[out_idx + 3] = recon_a;
                else if (bpp == 3) m_pixelData[out_idx + 3] = 255;
            }
        }
        m_valid = true;
    }

    bool IsValid() const { return m_valid; }
    uint32_t GetWidth() const { return m_width; }
    uint32_t GetHeight() const { return m_height; }
    const std::vector<uint8>& GetPixels() const { return m_pixelData; }

private:
    uint32_t read_big_endian_32(const uint8* buffer) {
        return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    }
    
    uint32_t m_width = 0, m_height = 0;
    std::vector<uint8> m_pixelData; // Always stored as RGBA
    bool m_valid = false;
};
