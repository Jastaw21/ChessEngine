//
// Created by jacks on 10/07/2025.
//

#ifndef MAGICBITBOARDS_H
#define MAGICBITBOARDS_H
#include <vector>

#include "MagicBitBoardShared.h"
#include "BoardManager/BitBoards.h"
#include "BoardManager/Rules.h"


struct Magic {
    Bitboard mask;
    Bitboard magic;
    int shift;
    std::vector<Bitboard> attacks;
};

class MagicBitBoards {
    Magic rookMagics[64];
    Magic bishopMagics[64];

    MBBHelpers mbbHelpers;

    const Bitboard rookMagicNumbers[64] = {
                0x2C80043180204004ULL, // 0
                0x14000402002100CULL, // 1
                0x100104409002000ULL, // 2
                0x5100201001000408ULL, // 3
                0x3200020110042008ULL, // 4
                0x8100010002040008ULL, // 5
                0x400088114500208ULL, // 6
                0x100148142002100ULL, // 7
                0x20800020804000ULL, // 8
                0x6000401000402000ULL, // 9
                0x81004011002000ULL, // 10
                0x4402000820401200ULL, // 11
                0x4041801800040180ULL, // 12
                0x880800C004A00ULL, // 13
                0x266808021000200ULL, // 14
                0x1002045830002ULL, // 15
                0x480004000402004ULL, // 16
                0x250064004200040ULL, // 17
                0x1010040102006ULL, // 18
                0x808008001000ULL, // 19
                0x50008005101ULL, // 20
                0x8001010004000208ULL, // 21
                0x10004001930084AULL, // 22
                0x600020001205084ULL, // 23
                0x62C0008080002040ULL, // 24
                0x40500140022002ULL, // 25
                0x210200080100080ULL, // 26
                0x3A05000900201000ULL, // 27
                0x92E8002500081100ULL, // 28
                0x1000020080040080ULL, // 29
                0x188020400100108ULL, // 30
                0x84422082000B1044ULL, // 31
                0x408001002108ULL, // 32
                0x10002000404004ULL, // 33
                0x100080802000ULL, // 34
                0x800801000800804ULL, // 35
                0x20040080800800ULL, // 36
                0x1070800200800400ULL, // 37
                0x2080244005001ULL, // 38
                0x302800040800100ULL, // 39
                0x8640400080208000ULL, // 40
                0x820100220040ULL, // 41
                0x8010008020008012ULL, // 42
                0x2110000804004040ULL, // 43
                0xA0A000810220004ULL, // 44
                0x4000402008080ULL, // 45
                0x42000801420004ULL, // 46
                0x20021484024A0001ULL, // 47
                0x3080002000400040ULL, // 48
                0x802000400180ULL, // 49
                0x4781802001100480ULL, // 50
                0x200084012002200ULL, // 51
                0x4204020800800480ULL, // 52
                0x42001108040200ULL, // 53
                0x4404C81002211400ULL, // 54
                0x9A00046081040600ULL, // 55
                0x5408000992101ULL, // 56
                0x4649181024001ULL, // 57
                0x20400811002001ULL, // 58
                0x288A19000080501ULL, // 59
                0x1001048004205ULL, // 60
                0x5000400080A25ULL, // 61
                0x100009002310804ULL, // 62
                0x102442C0082ULL // 63
            };

    const Bitboard bishopMagicNumbers[64] = {
                0x4040120A12020411ULL, // 0
                0x8004042800510110ULL, // 1
                0x112009403000480ULL, // 2
                0x2080A4303000010ULL, // 3
                0x1801104000480240ULL, // 4
                0x6010821040010028ULL, // 5
                0x30406A00CA300ULL, // 6
                0xA3008070080400ULL, // 7
                0x801400408220059ULL, // 8
                0xE001100138490040ULL, // 9
                0x7020090A0C011100ULL, // 10
                0x4801840429880414ULL, // 11
                0x900240420004000ULL, // 12
                0x20842480002ULL, // 13
                0xC024008404600404ULL, // 14
                0x4000008069082000ULL, // 15
                0x140820469080100ULL, // 16
                0x8430210040080ULL, // 17
                0x3010040111002100ULL, // 18
                0x104420220A020001ULL, // 19
                0x800820040208020AULL, // 20
                0x800140120310A000ULL, // 21
                0x3010A1608022208ULL, // 22
                0x21048202A021002ULL, // 23
                0x20500608110108ULL, // 24
                0x48600258020480ULL, // 25
                0x20280830008620ULL, // 26
                0x4002002408008020ULL, // 27
                0x1001001004000ULL, // 28
                0x10040914820100ULL, // 29
                0x202004414010802ULL, // 30
                0x20410002111102ULL, // 31
                0x44046004062000ULL, // 32
                0xD0A80800041000ULL, // 33
                0x820240400C0104ULL, // 34
                0x20080080080ULL, // 35
                0x2C0208020720020ULL, // 36
                0x28100020090080ULL, // 37
                0x2888D2140040212ULL, // 38
                0x43009A0210008481ULL, // 39
                0x2001014820094000ULL, // 40
                0x2244880450021302ULL, // 41
                0x48A09004A800ULL, // 42
                0x2002444010420200ULL, // 43
                0x2202341401400ULL, // 44
                0x8418C081006200ULL, // 45
                0x200C0C10410280ULL, // 46
                0x2404040062010042ULL, // 47
                0x817A20820442202ULL, // 48
                0x2800420090088001ULL, // 49
                0x200020211040804ULL, // 50
                0x20C0400020881300ULL, // 51
                0x4E829002022002ULL, // 52
                0x800201A02820900ULL, // 53
                0x828181810BC0038ULL, // 54
                0x6004012200200CULL, // 55
                0x8010400410410C0ULL, // 56
                0x40802008201B011ULL, // 57
                0x22C5000090480821ULL, // 58
                0x8000480000842400ULL, // 59
                0xB20100864A08200ULL, // 60
                0x20000008A2A80204ULL, // 61
                0x1800100528080040ULL, // 62
                0xC0500C0304C011ULL // 63
            };

    void initRookMagics();
    void initBishopMagics();

public:

    MagicBitBoards(){ init(); }

    void init(){
        initRookMagics();
        initBishopMagics();
    }

    Bitboard getRookAttacks(int square, Bitboard occupancy) const;
    Bitboard getBishopAttacks(int square, Bitboard occupancy) const;

    Bitboard getMoves(int square, const Piece& piece, const BitBoards& boards);
    void getMoves(int square, const Piece& piece, const BitBoards& boards, Bitboard& resultMoves);

    Rules rules;
    Bitboard findAttacksForColour(const Colours& colourToGetAttacksFor, const BitBoards& boards);
    Bitboard findAttacksForPiece(Piece piece, const BitBoards& boards);
    Bitboard getCastling(int square, const Piece& piece, const BitBoards& boards);

private:

    Bitboard calculateAttacksForPiece(int square, const Piece& piece, const BitBoards& boards);
};


#endif //MAGICBITBOARDS_H