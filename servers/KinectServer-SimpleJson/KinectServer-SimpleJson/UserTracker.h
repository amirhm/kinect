//
//  UserTracker.h
//  KinectServer1
//
//  Created by Timothy Prepscius on 3/20/13.
//  Copyright (c) 2013 Timothy Prepscius. All rights reserved.
//

#ifndef __KinectServer1__UserTracker__
#define __KinectServer1__UserTracker__

#include "NiTE.h"
#include <json/elements.h>
#include <stdexcept>
#include <vector>

#define MAX_USERS 10

class UserTracker
{
protected:
    bool g_visibleUsers[MAX_USERS] = {false};
    nite::SkeletonState g_skeletonStates[MAX_USERS] = {nite::SKELETON_NONE};

    nite::UserTracker userTracker;
    nite::UserTrackerFrameRef userTrackerFrame;
    typedef std::runtime_error Exception;
    
    json::Array jusers;

protected:
    void updateUserState(const nite::UserData& user, unsigned long long ts);
    
public:
    void initialize();
    void tick ();
    void shutdown();
        
    const json::Array &getUsers() const
    {
        return jusers;
    }
} ;


#endif /* defined(__KinectServer1__UserTracker__) */
