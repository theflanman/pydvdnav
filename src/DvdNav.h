//
// Created by connor on 6/30/24.
//

#ifndef PYDVDNAV_DVDNAV_H
#define PYDVDNAV_DVDNAV_H

#include <string>

#include "dvdnav/dvdnav.h"


class DvdNavException : public std::exception {
private: std::string message;
public: DvdNavException(std::string);
    std::string getMessage() const;
    char *what();
};

typedef std::tuple<uint8_t*, int32_t, int32_t> blockTuple;
typedef std::tuple<int32_t, int32_t> currentTitleInfoTuple;
typedef std::tuple<int32_t, int32_t, int32_t> currentTitleProgramTuple;
typedef std::tuple<int32_t, int32_t> positionTuple;


/**
 *
 */
class DvdNav {
private:
    void handle_status(dvdnav_status_t);

    dvdnav_t *_dvdnav;
    uint8_t mem[DVD_VIDEO_LB_LEN];
    int finished, output_fd, dump, tt_dump;

public:
    explicit DvdNav(char*);
    ~DvdNav();

    void reset();
    std::string getPath();

    void setRegionMask(int32_t);
    int32_t getRegionMask();
    void setReadaheadFlag(int32_t);
    int32_t getReadaheadFlag();
    void setPgcPositionFlag(int32_t);
    int32_t getPgcPositionFlag();

    blockTuple getNextBlock();
//    [uint8_t**, int32_t, int32_t] getNextCacheBlock();
    void freeCacheBlock(unsigned char*);
    void stillSkip();
    void waitSkip();
    uint32_t getNextStillFlag();
    void stop();

    uint8_t* getMem();

    currentTitleInfoTuple getCurrentTitleInfo();
    currentTitleProgramTuple getCurrentTitleProgram();
    positionTuple getPosition();

    pci_t getCurrentNavPCI();
    dsi_t getCurrentNavDSI();

    void selectAndActivateButton(int32_t button);

    int32_t getTitles();
    int32_t getParts(int32_t title);
    int32_t getAngles(int32_t title);

    void handle_status_weak(dvdnav_status_t status);

    void handle_status(dvdnav_status_t status, bool strong);
};


#endif //PYDVDNAV_DVDNAV_H
