#include <iostream>
#include <chrono>

typedef struct{
    unsigned short states[12];
    unsigned char count = 0;
} reachable;

bool stages[32][65536] = {false};
reachable reachability[65536];
bool reachabilityFilled = false;

//find whether state A can be transformed into state B by moving a single legal move
bool canReach(unsigned short A, unsigned short B) {
    if (reachabilityFilled){
        for (int i = 0; i < reachability[A].count; i++) {
            if (B == reachability[A].states[i]) return true;
        }
        return false;
    }
    //Check that only 1 disk moved
    int changeCount = 0;
    int movedDisk;
    for (int i = 0; i < 8; i++) {
        if (((A >> (i*2)) & 0x0003) != ((B >> (i*2)) & 0x0003)) {
            movedDisk = i;
            changeCount++;
        }
    }
    if (changeCount != 1) return false;

    //Check whether the single move is legal
    //Check if moved disk is on top
    for (int i = movedDisk + 1; i < 8; i++) {
        if (((A >> (i*2)) & 0x0003) == (((A >> (movedDisk*2))) & 0x0003)) return false;
    }
    //Check if destination peg is legal
    for (int i = movedDisk + 1; i < 8; i++) {
        if (((A >> (i*2)) & 0x0003) == (((B >> (movedDisk*2))) & 0x0003)) return false;
    }
    return true;

}


void fillReachability() {
    for (int i = 0; i < 65536; i++) {
        for (int j = 0; j < 65536; j++) {
            if (canReach(i, j)) {
                if (reachability[i].count == 12) {
                    std::cout << "ERROR" << '\n';
                    system("pause");
                }
                reachability[i].states[reachability[i].count] = j;
                reachability[i].count++;
            }
        }
    }
    reachabilityFilled = true;
}


int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "Hello\n";
    unsigned short startingState = 0;
    unsigned short endingState = 0xFFFF;
    fillReachability();
    std::cout << "Reachability Filled\n";
    for (int i = 0; i < 65536; i++) {
        if (canReach(startingState, i)) stages[0][i] = true;
    }
    for (int stage = 1; stage < 32; stage++) {
        for (int state1 = 0; state1 < 65536; state1++){
            if (stages[stage-1][state1]) {
                for (int state2 = 0; state2 < 65536; state2++) {
                    if (canReach(state1, state2)) stages[stage][state2] = true;
                }
            }
        }
        std::cout << "Finished Stage " << stage << " forward\n";
    }
    for (int i = 0; i < 65536; i++) {
        if (!canReach(i, endingState)) stages[31][i] = false;
    }
    for (int stage = 30; stage >= 0; stage--) {
        for (int state1 = 0; state1 < 65536; state1++){
            if (stages[stage][state1]) {
                stages[stage][state1] = false;
                for (int state2 = 0; state2 < 65536; state2++) {
                    if (stages[stage+1][state2] == false) continue;
                    if (canReach(state1, state2)) {
                        stages[stage][state1] = true;
                        break;
                    }
                }
            }
        }
        std::cout << "Finished Stage " << stage << " backward\n";
    }

    bool validStages[32] = {false};
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 65536; j++) {
            if (stages[i][j]) {
                validStages[i] = true;
                break;
            }
        }
    }

    unsigned short prevState = startingState;
    int movedDisk;
    for (int i = 0; i < 32; i++) {
        std::cout << "Move " << i+1 << ": ";
        for (int j = 0; j < 65536; j++) {
            if (stages[i][j] && canReach(prevState, j)) {
                for (int i = 0; i < 8; i++) {
                    if (((prevState >> (i*2)) & 0x0003) != ((j >> (i*2)) & 0x0003)) {
                        movedDisk = i;
                    }
                }
                std::cout << "from " << ((prevState >> (movedDisk*2)) & 0x0003) + 1 << " to " << ((j >> (movedDisk*2)) & 0x0003) + 1;
                prevState = j;
                break;
            }
        }
        std::cout << '\n';
    }
    std::cout << "Move " << 33 << ": ";
    for (int i = 0; i < 8; i++) {
        if (((prevState >> (i*2)) & 0x0003) != ((endingState >> (i*2)) & 0x0003)) {
            movedDisk = i;
        }
    }
    std::cout << "from " << ((prevState >> (movedDisk*2)) & 0x0003) + 1 << " to " << ((endingState >> (movedDisk*2)) & 0x0003) + 1;
    std::cout << '\n';

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    std::cout << "Program took " << duration.count() << " seconds" << std::endl;


    system("pause");
}