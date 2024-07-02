//
// Created by connor on 6/30/24.
//

#include "DvdNav.h"

#include <utility>

#include <cstring>
#include <stdexcept>
#include <tuple>

#define DVD_READ_CACHE 1
#define DVD_LANGUAGE "en"

DvdNav::DvdNav(char *disk_path) {

    printf("test %s\n", disk_path);

    handle_status(dvdnav_open(&(this->_dvdnav), disk_path));

    handle_status(dvdnav_set_readahead_flag(this->_dvdnav, DVD_READ_CACHE));
    handle_status(dvdnav_menu_language_select(this->_dvdnav, DVD_LANGUAGE));
    handle_status(dvdnav_audio_language_select(this->_dvdnav, DVD_LANGUAGE));
    handle_status(dvdnav_spu_language_select(this->_dvdnav, DVD_LANGUAGE));
    handle_status(dvdnav_set_PGC_positioning_flag(this->_dvdnav, 1));

    this->dump = 0;
    this->finished = 0;
    this->tt_dump = 0;
    this->output_fd = 0;

}
void DvdNav::handle_status(dvdnav_status_t status, bool strong) {
    if (strong) {
        this->handle_status(status);
    } else {
        this->handle_status_weak(status);
    }
}

void DvdNav::handle_status(dvdnav_status_t status) {

    switch (status) {
        case DVDNAV_STATUS_OK:
            break;
        case DVDNAV_STATUS_ERR:
            // throw DvdNavException(dvdnav_err_to_string(this->_dvdnav));
            printf("Exception: %s\n", dvdnav_err_to_string(this->_dvdnav));
            break;
        default:
            throw std::runtime_error("");
    }

}

void DvdNav::handle_status_weak(dvdnav_status_t status) {

    switch (status) {
        case DVDNAV_STATUS_OK:
            break;
        case DVDNAV_STATUS_ERR:
            break;
        default:
            throw std::runtime_error("");
    }

}

DvdNav::~DvdNav() {

//    this->handle_status(dvdnav_close(this->_dvdnav));

}

/**
 * Fills a pointer with a value pointing to a string describing
 * the path associated with an open dvdnav_t. It assigns *path to NULL
 * on error.
 */
std::string DvdNav::getPath() {
    const char *path;
    this->handle_status(dvdnav_path(this->_dvdnav, &path));
    return path;
}

void DvdNav::reset() {

    this->handle_status(dvdnav_reset(this->_dvdnav));

}

/**
 * Sets the region mask (bit 0 set implies region 1, bit 1 set implies
 * region 2, etc) of the virtual machine. Generally you will only need to set
 * this if you are playing RCE discs which query the virtual machine as to its
 * region setting.
 *
 * This has _nothing_ to do with the region setting of the DVD drive.
 */
void DvdNav::setRegionMask(int32_t region_mask) { dvdnav_set_region_mask(this->_dvdnav, region_mask); }

/**
 * Returns the region mask (bit 0 set implies region 1, bit 1 set implies
 * region 2, etc) of the virtual machine.
 *
 * This has _nothing_ to do with the region setting of the DVD drive.
 */
int32_t DvdNav::getRegionMask() {
    int32_t regionMask = 0;
    this->handle_status(dvdnav_get_region_mask(this->_dvdnav, &regionMask));
    return regionMask;
}

/**
 * Specify whether read-ahead caching should be used. You may not want this if your
 * decoding engine does its own buffering.
 *
 * The default read-ahead cache does not use an additional thread for the reading
 * (see read_cache.c for a threaded cache, but note that this code is currently
 * unmaintained). It prebuffers on VOBU level by reading ahead several buffers
 * on every read request. The speed of this prebuffering has been optimized to
 * also work on slow DVD drives.
 *
 * If in addition you want to prevent memcpy's to improve performance, have a look
 * at dvdnav_get_next_cache_block().
 */
void DvdNav::setReadaheadFlag(int32_t regionMask) {
    this->handle_status(dvdnav_set_region_mask(this->_dvdnav, regionMask));
}

/**
 * Query whether read-ahead caching/buffering will be used.
 */
int32_t DvdNav::getReadaheadFlag() {
    return 0;
}

/**
 * Specify whether the positioning works PGC or PG based.
 * Programs (PGs) on DVDs are similar to Chapters and a program chain (PGC)
 * usually covers a whole feature. This affects the behaviour of the
 * functions dvdnav_get_position() and dvdnav_sector_search(). See there.
 * Default is PG based positioning.
 */
void DvdNav::setPgcPositionFlag(int32_t) {

}

/**
 * Query whether positioning is PG or PGC based.
 */
int32_t DvdNav::getPgcPositionFlag() {
    return 0;
}

void DvdNav::freeCacheBlock(unsigned char *) {
    throw stderr;
}

void DvdNav::stillSkip() {
    this->handle_status(dvdnav_still_skip(this->_dvdnav));
}

void DvdNav::waitSkip() {
    this->handle_status(dvdnav_wait_skip(this->_dvdnav));
}

uint32_t DvdNav::getNextStillFlag() {
    return 0;
}

void DvdNav::stop() {

}

std::tuple<uint8_t *, int32_t, int32_t> DvdNav::getNextBlock() {
    int32_t event, len;
    this->handle_status(dvdnav_get_next_block(this->_dvdnav, this->mem, &event, &len));
    return {this->mem, event, len};
}

uint8_t *DvdNav::getMem() {
    return this->mem;
}

/**
 * Return the title number and part currently being played.
 * A title of 0 indicates we are in a menu. In this case, part
 * is set to the current menu's ID.
 */
currentTitleInfoTuple DvdNav::getCurrentTitleInfo() {
    int32_t title = 0, part = 0;
    this->handle_status(dvdnav_current_title_info(this->_dvdnav, &title, &part));
    return {title, part};
}

/**
 * Return the title number, pgcn and pgn currently being played.
 * A title of 0 indicates, we are in a menu.
 */
currentTitleProgramTuple DvdNav::getCurrentTitleProgram() {
    int32_t title = 0, pgcn = 0, pgn = 0;
    this->handle_status(dvdnav_current_title_program(this->_dvdnav, &title, &pgcn, &pgn), false);
    return {title, pgcn, pgn};
}

positionTuple DvdNav::getPosition() {
    uint32_t pos, len;
    this->handle_status(dvdnav_get_position(this->_dvdnav, &pos, &len));
    return {pos, len};
}

pci_t DvdNav::getCurrentNavPCI() {
    return *dvdnav_get_current_nav_pci(this->_dvdnav);
}

dsi_t DvdNav::getCurrentNavDSI() {
    return *dvdnav_get_current_nav_dsi(this->_dvdnav);
}

void DvdNav::selectAndActivateButton(int32_t button) {
    this->handle_status(dvdnav_button_select_and_activate(this->_dvdnav, dvdnav_get_current_nav_pci(this->_dvdnav), button));
}

int32_t DvdNav::getTitles() {
    int32_t titles = -1;
    this->handle_status(dvdnav_get_number_of_titles(this->_dvdnav, &titles));
    return titles;
}

int32_t DvdNav::getParts(int32_t title) {
    int32_t parts = -1;
    this->handle_status(dvdnav_get_number_of_parts(this->_dvdnav, title, &parts));
    return parts;
}

int32_t DvdNav::getAngles(int32_t title) {
    int32_t angles = -1;
    this->handle_status(dvdnav_get_number_of_angles(this->_dvdnav, title, &angles));
    return angles;
}

DvdNavException::DvdNavException(std::string message) {
    this->message = std::move(message);
}

char *DvdNavException::what() {
    char *c_message = static_cast<char *>(malloc(sizeof(char)));
    strcpy(c_message, this->message.c_str());
    return c_message;
}

std::string DvdNavException::getMessage() const {
    return this->message;
}
