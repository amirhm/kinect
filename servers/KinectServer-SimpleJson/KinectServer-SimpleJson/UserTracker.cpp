/*******************************************************************************
 *                                                                              *
 *   PrimeSense NiTE 2.0 - Simple Skeleton Sample                               *
 *   Copyright (C) 2012 PrimeSense Ltd.                                         *
 *                                                                              *
 *******************************************************************************/

#include "UserTracker.h"

#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

const char *JOINT_STRINGS[] = {
    
    "head", //JOINT_HEAD,
    "neck", // JOINT_NECK,
    
    "lshoulder", // JOINT_LEFT_SHOULDER,
    "rshoulder", // JOINT_RIGHT_SHOULDER,
    "lelbow", // JOINT_LEFT_ELBOW,
    "relbow", // JOINT_RIGHT_ELBOW,
    "lhand", // JOINT_LEFT_HAND,
    "rhand", //JOINT_RIGHT_HAND,
    
    "torso", //JOINT_TORSO,
    
    "lhip", // JOINT_LEFT_HIP,
    "rhip", // JOINT_RIGHT_HIP,
    "lknee", // JOINT_LEFT_KNEE,
    "rknee", // JOINT_RIGHT_KNEE,
    "lfoot", // JOINT_LEFT_FOOT,
    "rfoot" // JOINT_RIGHT_FOOT,
};

int wasKeyboardHit()
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    // don't echo and don't wait for ENTER
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    
    // make it non-blocking (so we can check without waiting)
    if (0 != fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK))
    {
        return 0;
    }
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    if (0 != fcntl(STDIN_FILENO, F_SETFL, oldf))
    {
        return 0;
    }
    
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}


#define USER_MESSAGE(msg) \
{printf("[%08llu] User #%d:\t%s\n",ts, user.getId(),msg);}

void UserTracker::updateUserState(const nite::UserData& user, unsigned long long ts)
{
	if (user.isNew())
		USER_MESSAGE("New")
        else if (user.isVisible() && !g_visibleUsers[user.getId()])
            USER_MESSAGE("Visible")
            else if (!user.isVisible() && g_visibleUsers[user.getId()])
                USER_MESSAGE("Out of Scene")
                else if (user.isLost())
                    USER_MESSAGE("Lost")
                    
                    g_visibleUsers[user.getId()] = user.isVisible();
    
    
	if(g_skeletonStates[user.getId()] != user.getSkeleton().getState())
	{
		switch(g_skeletonStates[user.getId()] = user.getSkeleton().getState())
		{
            case nite::SKELETON_NONE:
                USER_MESSAGE("Stopped tracking.")
                break;
            case nite::SKELETON_CALIBRATING:
                USER_MESSAGE("Calibrating...")
                break;
            case nite::SKELETON_TRACKED:
                USER_MESSAGE("Tracking!")
                break;
            case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
            case nite::SKELETON_CALIBRATION_ERROR_HANDS:
            case nite::SKELETON_CALIBRATION_ERROR_LEGS:
            case nite::SKELETON_CALIBRATION_ERROR_HEAD:
            case nite::SKELETON_CALIBRATION_ERROR_TORSO:
                USER_MESSAGE("Calibration Failed... :-|")
                break;
		}
	}
}

void UserTracker::initialize()
{
    nite::Status niteRc;
    
    nite::NiTE::initialize();
    
    niteRc = userTracker.create();
    if (niteRc != nite::STATUS_OK)
        throw Exception("Couldn't create user tracker\n");
    
    printf("\nStart moving around to get detected...\n(PSI pose may be required for skeleton calibration, depending on the configuration)\n");
}

void UserTracker::tick ()
{
    nite::Status niteRc;
    {
        niteRc = userTracker.readFrame(&userTrackerFrame);
        if (niteRc != nite::STATUS_OK)
        {
            printf("Get next frame failed\n");
            return;
        }
        
        jusers.Clear();
        
        const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
        for (int i = 0; i < users.getSize(); ++i)
        {
            const nite::UserData& user = users[i];
            updateUserState(user,userTrackerFrame.getTimestamp());
            if (user.isNew())
            {
                userTracker.startSkeletonTracking(user.getId());
            }
            else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
            {
                json::Object juser;

                for (int j=0; j<NITE_JOINT_COUNT; ++j)
                {
                    const nite::SkeletonJoint& joint = user.getSkeleton().getJoint((nite::JointType)j);

                    json::Object jjoint;
                    json::Array jpos;
                    jpos.Insert(json::Number(joint.getPosition().x));
                    jpos.Insert(json::Number(joint.getPosition().y));
                    jpos.Insert(json::Number(joint.getPosition().z));
                    
                    json::Array jquat;
                    jquat.Insert(json::Number(joint.getOrientation().x));
                    jquat.Insert(json::Number(joint.getOrientation().y));
                    jquat.Insert(json::Number(joint.getOrientation().z));
                    jquat.Insert(json::Number(joint.getOrientation().w));
                    
                    jjoint["c"] = json::Number(joint.getPositionConfidence());
                    jjoint["p"] = jpos;
                    jjoint["r"] = jquat;
                    
                    juser[JOINT_STRINGS[j]] = jjoint;
                }
                
                jusers.Insert(juser);
            }
        }
        
    }
}

void UserTracker::shutdown()
{
    nite::NiTE::shutdown();    
}


