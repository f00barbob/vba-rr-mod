local save_path = "z:/" -- don't forget to escape backslashes, or use frontslashes instead
local save_prefix = "save"
local save_ext = "sgm"

-- the result are save files with a naming format like "Z:\save11370049985.sgm"
-- the timestamps are so-called epoch times.

local save_interval = 20  -- in minutes
local save_interval = save_interval * 60

local did_save = false

keyBindings	= {
	keyMute	= "M",
	keyUnmute  = "N"
}

base_time = os.time()  -- get start time in seconds since Jan 1 1970

while true do    
    
    inpto	= inpt;
	inpt	= input.get()
    
    -- not neccessary, but these prevent the effects of a key being held continuously for more than 1 frame.
    
	if  inpt[keyBindings['keyMute']] and not inpto[keyBindings['keyMute']] then
        emu.mute()
	end
    
	if  inpt[keyBindings['keyUnmute']] and not inpto[keyBindings['keyUnmute']] then
        emu.unmute()
	end
    
    -- subtract base time from current; modulo per the interval 
    current_interval = (os.time() - base_time) % save_interval

    -- if the remainder of the above is 0, save it and set the flag to prevent 3600 saves
    --  (1 per frame from for one minute) from being made
    if current_interval == 0
    and did_save == false then
        hax.save(save_path .. "\\" .. save_prefix .. os.time() .. "." .. save_ext)
        did_save = true
    elseif current_interval ~= 0 then -- once the remainder is non-zero, reset the flag
        did_save = false 
    end
        
        
    
    emu.frameadvance()
    
end
