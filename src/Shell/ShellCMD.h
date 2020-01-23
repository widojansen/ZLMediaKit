//
// Created by xzl on 2017/12/1.
// Translated WJ 20200123

#ifndef SRC_SHELL_SHELLCMD_H_
#define SRC_SHELL_SHELLCMD_H_

#include "Util / CMD.h"
#include "Common / MediaSource.h"
using namespace toolkit;

namespace mediakit {


class CMD_media: public CMD {
public:
    CMD_media () {
        _parser.reset (new OptionParser ([] (const std :: shared_ptr <ostream> & stream, mINI & ini) {
            MediaSource :: for_each_media ([&] (const MediaSource :: Ptr & media) {
                if (! ini ["schema"]. empty () && ini ["schema"]! = media-> getSchema ()) {
                    // Screening protocols do not match
                    return;
                }
                if (! ini ["vhost"]. empty () && ini ["vhost"]! = media-> getVhost ()) {
                    // Filtering virtual hosts does not match
                    return;
                }
                if (! ini ["app"]. empty () && ini ["app"]! = media-> getApp ()) {
                    // Filter application names that do not match
                    return;
                }
                if (! ini ["stream"]. empty () && ini ["stream"]! = media-> getId ()) {
                    // stream id does not match
                    return;
                }
                if (ini.find ("list")! = ini.end ()) {
                    // List sources
                    (* stream) << "\ t"
                              << media-> getSchema () << "/"
                              << media-> getVhost () << "/"
                              << media-> getApp () << "/"
                              << media-> getId ()
                              << "\ r \ n";
                    return;
                }

                EventPollerPool :: Instance (). GetPoller ()-> async ([ini, media, stream] () {
                    if (ini.find ("kick")! = ini.end ()) {
                        // kick out the source
                        do {
                            if (! media) {
                                break;
                            }
                            if (! media-> close (true)) {
                                break;
                            }
                            (* stream) << "\ tKicked out successfully:"
                                      << media-> getSchema () << "/"
                                      << media-> getVhost () << "/"
                                      << media-> getApp () << "/"
                                      << media-> getId ()
                                      << "\ r \ n";
                            return;
                        } while (0);
                        (* stream) << "\ t kick out failed:"
                                  << media-> getSchema () << "/"
                                  << media-> getVhost () << "/"
                                  << media-> getApp () << "/"
                                  << media-> getId ()
                                  << "\ r \ n";
                    }
                }, false);


            });
        }));
        (* _parser) << Option ('k', "kick", Option :: ArgNone, nullptr, false, "Kick Out Media Source", nullptr);
        (* _parser) << Option ('l', "list", Option :: ArgNone, nullptr, false, "List media sources", nullptr);
        (* _parser) << Option ('S', "schema", Option :: ArgRequired, nullptr, false, "Protocol Filter", nullptr);
        (* _parser) << Option ('v', "vhost", Option :: ArgRequired, nullptr, false, "Virtual Host Filter", nullptr);
        (* _parser) << Option ('a', "app", Option :: ArgRequired, nullptr, false, "Application name filtering", nullptr);
        (* _parser) << Option ('s', "stream", Option :: ArgRequired, nullptr, false, "Stream id filtering", nullptr);
    }
    virtual ~ CMD_media () {}
    const char * description () const override {
        return "Media source related operations.";
    }
};

} / * namespace mediakit * /

#endif // SRC_SHELL_SHELLCMD_H_