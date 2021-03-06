/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/


#include "renderer/CCGroupCommand.h"
#include "renderer/CCRenderer.h"
#include "CCDirector.h"

NS_CC_BEGIN

static GroupCommandManager* s_instance;
GroupCommandManager *GroupCommandManager::getInstance()
{
    if(!s_instance)
    {
        s_instance = new GroupCommandManager();
        if(!s_instance->init())
        {
            CC_SAFE_DELETE(s_instance);
        }
    }
    return s_instance;
}

GroupCommandManager::GroupCommandManager()
{

}

GroupCommandManager::~GroupCommandManager()
{
    CC_SAFE_RELEASE_NULL(s_instance);
}

bool GroupCommandManager::init()
{
    //0 is the default render group
    _groupMapping[0] = true;
    return true;
}

int GroupCommandManager::getGroupID()
{
    //Reuse old id
    for(auto it = _groupMapping.begin(); it != _groupMapping.end(); ++it)
    {
        if(!it->second)
        {
            _groupMapping[it->first] = true;
            return it->first;
        }
    }

    //Create new ID
//    int newID = _groupMapping.size();
    int newID = Director::getInstance()->getRenderer()->createRenderQueue();
    _groupMapping[newID] = true;

    return newID;
}

void GroupCommandManager::releaseGroupID(int groupID)
{
    _groupMapping[groupID] = false;
}

GroupCommand::GroupCommand()
: _viewport(0)
, _depth(0)
{
    _type = RenderCommand::Type::GROUP_COMMAND;
    _renderQueueID = GroupCommandManager::getInstance()->getGroupID();
}

void GroupCommand::init(int viewport, int32_t depth)
{
    _viewport = viewport;
    _depth = depth;
    GroupCommandManager::getInstance()->releaseGroupID(_renderQueueID);
    _renderQueueID = GroupCommandManager::getInstance()->getGroupID();
}

GroupCommand::~GroupCommand()
{
    GroupCommandManager::getInstance()->releaseGroupID(_renderQueueID);
}

int64_t GroupCommand::generateID()
{
    _id = 0;

    _id = (int64_t)_viewport << 61
            | (int64_t)1 << 60 // translucent
            | (int64_t)_depth << 36;

    return _id;
}

NS_CC_END
