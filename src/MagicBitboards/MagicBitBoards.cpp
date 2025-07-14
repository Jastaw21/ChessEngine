
//
// Created by jacks on 10/07/2025.
//

#include "MagicBitboards/MagicBitBoards.h"

void MagicBitBoards::initRookMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = rookMagics[square];
        magic.mask = mbbHelpers.generateRookMask(square); // Use precomputed magic
        magic.magic = rookMagicNumbers[square];
        magic.shift = 64 - std::popcount(magic.mask); // keep relevant bits

        const int attacksSize = 1 << std::popcount(magic.mask); // how many attacks are there? shifting is powers
        magic.attacks.resize(attacksSize); // the max size of the table is the number of attacks

        for (int i = 0; i < attacksSize; i++) {
            Bitboard occupancy = mbbHelpers.getOccupancyFromIndex(i, magic.mask);
            const Bitboard attacks = mbbHelpers.getRookAttacks(square, occupancy);

            occupancy *= magic.magic; // apply the magic number
            occupancy >>= magic.shift; // shift it down
            magic.attacks[occupancy] = attacks;
        }
    }
}

void MagicBitBoards::initBishopMagics(){
    for (int square = 0; square < 64; square++) {
        Magic& magic = bishopMagics[square];
        magic.mask = mbbHelpers.generateBishopMask(square);
        magic.magic = bishopMagicNumbers[square]; // Use precomputed magic
        magic.shift = 64 - std::popcount(magic.mask); // keep relevant bits

        const int attacksSize = 1 << std::popcount(magic.mask); // how many attacks are there? shifting is powers
        magic.attacks.resize(attacksSize); // the max size of the table is the number of attacks

        for (int i = 0; i < attacksSize; i++) {
            Bitboard occupancy = mbbHelpers.getOccupancyFromIndex(i, magic.mask);
            const Bitboard attacks = mbbHelpers.getBishopAttacks(square, occupancy);

            occupancy *= magic.magic; // apply the magic number
            occupancy >>= magic.shift; // shift it down
            magic.attacks[occupancy] = attacks; // [ occupancy ] = [ attacks
        }
    }
}

Bitboard MagicBitBoards::getRookAttacks(const int square, Bitboard occupancy) const{
    const Magic& magic = rookMagics[square];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}

Bitboard MagicBitBoards::getBishopAttacks(const int square, Bitboard occupancy) const{
    const Magic& magic = bishopMagics[square];
    occupancy &= magic.mask;
    occupancy *= magic.magic;
    occupancy >>= magic.shift;
    return magic.attacks[occupancy];
}

Bitboard MagicBitBoards::getMoves(const int square, const Piece& piece, const BitBoards& boards){
    switch (piece) {
        case WR:
        case BR:
            return getRookAttacks(square, boards.getOccupancy());
        case WB:
        case BB:
            return getBishopAttacks(square, boards.getOccupancy());
        case WQ:
        case BQ:
            return getRookAttacks(square, boards.getOccupancy()) | getBishopAttacks(square, boards.getOccupancy());
        case WN:
        case BN:
            return rules.knightAttacks[square] & ~boards.getOccupancy(pieceColours[piece]); // cant go to own square
        case WK:
        case BK: {
            const Bitboard castling = getCastling(square, piece, boards);
            return (castling | rules.kingMoves[square]) & ~boards.getOccupancy(pieceColours[piece]);
            break;
        }
        // cant go to own square
        case BP:
        case WP: {
            Bitboard pushes = rules.getPseudoPawnPushes(piece, square); // just get the dumb pushes

            // need to check the immediate next rank, this blocks us if we move 1 or 2 ranks
            const int blockingRankOffset = pieceColours[piece] == WHITE ? 1 : -1;
            int blockingRank = squareToRank(square) + blockingRankOffset;
            if (blockingRank > 0 && blockingRank < 9) {
                const int blockingSquare = rankAndFileToSquare(blockingRank, squareToFile(square));
                if (boards.testSquare(blockingSquare))
                    pushes = 0ULL;
            }
            // now check the second rank
            blockingRank += blockingRankOffset;
            if (blockingRank > 0 && blockingRank < 9) {
                const int blockingSquare = rankAndFileToSquare(blockingRank, squareToFile(square));
                if (boards.testSquare(blockingSquare))
                    pushes &= ~(1ULL << blockingSquare); // just blank that square off
            }

            const Bitboard rawMoves = rules.getPseudoPawnAttacks(piece, square) | pushes;
            const Piece opponentPawn = piece == BP ? WP : BP;
            const Bitboard epSquare = rules.getPseudoPawnEP(piece, square, boards.getOccupancy(opponentPawn));
            return (rawMoves | epSquare) & ~boards.getOccupancy(pieceColours[piece]);
        }
        default:
            return 0ULL;
    }
}

void MagicBitBoards::getMoves(const int square, const Piece& piece, const BitBoards& boards, Bitboard& resultMoves){
    switch (piece) {
        case WR:
        case BR: {
            resultMoves |= getRookAttacks(square, boards.getOccupancy());
            break;
        }
        case WB:
        case BB: {
            resultMoves |= getBishopAttacks(square, boards.getOccupancy());
            break;
        }
        case WQ:
        case BQ: {
            resultMoves |= getRookAttacks(square, boards.getOccupancy()) | getBishopAttacks(
                square, boards.getOccupancy());
            break;
        }
        case WN:
        case BN: {
            resultMoves |= rules.knightAttacks[square] & ~boards.getOccupancy(pieceColours[piece]);
            break;
        } // cant go to own square
        case WK:
        case BK: {
            const Bitboard castling = getCastling(square, piece, boards);
            resultMoves |= (castling | rules.kingMoves[square]) & ~boards.getOccupancy(pieceColours[piece]);
            break;
        }
        // cant go to own square
        case BP:
        case WP: {
            const Bitboard pushes = rules.getPseudoPawnPushes(piece, square); // just get the dumb pushes

            // need to check the immediate next rank, this blocks us if we move 1 or 2 ranks
            const int blockingRankOffset = pieceColours[piece] == WHITE ? 1 : -1;
            int blockingRank = squareToRank(square) + blockingRankOffset;
            if (blockingRank > 0 && blockingRank < 9) {
                const int blockingSquare = rankAndFileToSquare(blockingRank, squareToFile(square));
                if (boards.testSquare(blockingSquare))
                    resultMoves = 0ULL;
            }
            // now check the second rank
            blockingRank += blockingRankOffset;
            if (blockingRank > 0 && blockingRank < 9) {
                const int blockingSquare = rankAndFileToSquare(blockingRank, squareToFile(square));
                if (boards.testSquare(blockingSquare))
                    resultMoves &= ~(1ULL << blockingSquare); // just blank that square off
            }

            const Bitboard rawMoves = rules.getPseudoPawnAttacks(piece, square) | pushes;
            const Piece opponentPawn = piece == BP ? WP : BP;
            const Bitboard epSquare = rules.getPseudoPawnEP(piece, square, boards.getOccupancy(opponentPawn));
            resultMoves |= ((rawMoves | epSquare) & ~boards.getOccupancy(pieceColours[piece]));

            break;
        }
        default:
            resultMoves = 0ULL;
    }
}

Bitboard MagicBitBoards::getSimpleAttacks(const int square, const Piece& piece, const BitBoards& boards){
    switch (piece) {
        case WR:
        case BR:
            return getRookAttacks(square, boards.getOccupancy());
        case WB:
        case BB:
            return getBishopAttacks(square, boards.getOccupancy());
        case WQ:
        case BQ:
            return getRookAttacks(square, boards.getOccupancy()) | getBishopAttacks(square, boards.getOccupancy());
        case WN:
        case BN:
            return rules.knightAttacks[square] & ~boards.getOccupancy(pieceColours[piece]); // cant go to own square
        case WK:
        case BK:
            return rules.kingMoves[square] & ~boards.getOccupancy(pieceColours[piece]); // cant go to own square
        case BP:
        case WP:
            return rules.getPseudoPawnAttacks(piece, square) & ~boards.getOccupancy(pieceColours[piece]);
        default: ;
    }

    return 0ULL;
}

Bitboard MagicBitBoards::getAllAttacks(const Colours& colourToGetAttacksFor, const BitBoards& boards){
    Bitboard attacks = 0ULL;
    for (int piece = 0; piece < PIECE_N; ++piece) {
        const auto pieceToSearch = static_cast<Piece>(piece);
        if (pieceColours[pieceToSearch] == colourToGetAttacksFor) {
            Bitboard occupancy = boards.getOccupancy(pieceToSearch);

            while (occupancy) {
                const int square = popLowestSetBit(occupancy);
                attacks |= getSimpleAttacks(square, pieceToSearch, boards);
            }
        }
    }
    return attacks;
}

Bitboard MagicBitBoards::getCastling(const int square, const Piece& piece, const BitBoards& boards){
    if (piece != WK && piece != BK)
        return 0ULL;

    if (square != 4 && square != 60)
        return 0ULL;

    Bitboard result = 0ULL;

    const auto isQueenSide = {false, true};
    for (const auto& queenSide: isQueenSide) {
        auto castlingMask = queenSide ? Constants::QUEEN_SIDE_CASTLING : Constants::KING_SIDE_CASTLING;
        if (pieceColours[piece] == WHITE) { castlingMask &= Constants::RANK_1; } else {
            castlingMask &= Constants::RANK_8;
        }

        if (castlingMask & boards.getOccupancy())
            continue; // can't castle through an occupied square

        // now take file c out of the equation, as it doesn't matter if that's attacked
        castlingMask &= ~Constants::FILE_B;
        const auto attacks = getAllAttacks((pieceColours[piece] == WHITE ? BLACK : WHITE), boards);
        if (attacks & castlingMask)
            continue;

        if (pieceColours[piece] == WHITE) { result |= queenSide ? Constants::C1 : Constants::G1; } else {
            result |= queenSide ? Constants::C8 : Constants::G8;
        }
    }

    return result;
}
