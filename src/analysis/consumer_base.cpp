/*
 *
 *                This source code is part of
 *                    ******************
 *                    ***   Pteros   ***
 *                    ******************
 *                 molecular modeling library
 *
 * Copyright (c) 2009-2013, Semen Yesylevskyy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of Artistic License:
 *
 * Please note, that Artistic License is slightly more restrictive
 * then GPL license in terms of distributing the modified versions
 * of this software (they should be approved first).
 * Read http://www.opensource.org/licenses/artistic-license-2.0.php
 * for details. Such license fits scientific software better then
 * GPL because it prevents the distribution of bugged derivatives.
 *
*/


#include "pteros/analysis/consumer_base.h"
#include "pteros/analysis/trajectory_processor.h"
#include <boost/bind.hpp>

using namespace std;
using namespace pteros;

Consumer_base::Consumer_base(Trajectory_processor* pr){
    proc = pr;
    proc->add_consumer(this); // Add this consumer to trajectory processor        
}

void Consumer_base::run(){
    // Some info before running
    cout << "Consumer " << id << " works on system with " << system.num_atoms() << " atoms" << endl;
    // Allocate data container
    boost::shared_ptr<Data_container> data;    
    // Call user pre-processing code
    pre_process();
    // Process frames
    saved_time = -1.0;
    saved_abs_frame = -1;
    saved_valid_frame = -1;
    int i = 0;
    // Main loop
    for(;;){
        // Obtain data from the master thread
        data = proc->frame_provider(i,id);

        if(data->stop){
            // if we requested to stop, put last saved time into data
            // because this info is absent
            data->frame_info.absolute_time = saved_time;
            data->frame_info.valid_frame = saved_valid_frame;
            data->frame_info.absolute_frame = saved_abs_frame;
            // exit from main loop
            break;
        }

        saved_time = data->frame_info.absolute_time;
        saved_valid_frame = data->frame_info.valid_frame;
        saved_abs_frame = data->frame_info.absolute_frame;

        // Process window info
        process_frame_info(data->frame_info);

        // Here we process frame data, which arrived. Should defined in derived classes
        process_frame_data(data->frame);

        // Call user processing function
        bool ok = process_frame(data->frame_info);
        // If false returned, exit from main loop
        if(!ok) break;

        ++i;
    } // End of main loop!
    // Notify master thread that we don't need data any more
    proc->consumer_finished(id);
    // Close last window. If there is a single window, it will be closed as well.
    window_finished(data->frame_info);
    // Set final time and frame for post-processor
    data->frame_info.last_frame = data->frame_info.absolute_frame;
    data->frame_info.last_time = data->frame_info.absolute_time;
    // Call post-processing code (info from the last processed frame is passed)
    post_process(data->frame_info);
}

void Consumer_base::pre_process(){
}

bool Consumer_base::process_frame(const Frame_info& info){
    return true;
}

void Consumer_base::post_process(const Frame_info& info){
}

void Consumer_base::window_started(const Frame_info& info){
}


void Consumer_base::window_finished(const Frame_info& info){
}


void Consumer_base::process_frame_info(Frame_info& info){
    if(info.valid_frame==0){
        // This is the start of the very first window
        win_num = 0; // Set counter
        info.win_num = 0;
        info.win_start_frame = info.valid_frame;
        info.win_start_time = info.absolute_time;
        info.win_last_frame = info.valid_frame;
        info.win_last_time = info.absolute_time;
        // Call function for processing
        window_started(info);
    } else {
        // Check the end of window
        if(
            (info.win_size_frames>=0 && info.valid_frame-info.win_start_frame>=info.win_size_frames)
            ||
            (info.win_size_time>=0 && info.absolute_time-info.win_start_time>=info.win_size_time)
        ){
            // Previous window finished! Call function for processing.
            // It will see old win_start_time and win_num as needed.
            window_finished(info);
            // Start new window
            ++win_num;
            info.win_num = win_num;
            info.win_start_frame = info.valid_frame;
            info.win_start_time = info.absolute_time;
            info.win_last_frame = info.valid_frame;
            info.win_last_time = info.absolute_time;
            // Call function for processing
            window_started(info);
        } else {
            // Window if not finished yet. Just update last_time
            info.win_last_frame = info.valid_frame;
            info.win_last_time = info.absolute_time;
        }
    }
}