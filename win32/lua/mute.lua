local save_path = "z:/" -- don't forget to escape backslashes, or use frontslashes instead
local save_prefix = "save"
local save_ext = "sgm"

-- the result are save files with a naming format like "Z:\save11370049985.sgm"
-- the timestamps are so-called epoch times.

local save_interval = 1  -- in minutes
local save_interval = save_interval * 60

local did_save = false

keyBindings	= {
	keyMute	= "M",
	keyUnmute  = "N"
}

base_time = os.time()  -- get start time in seconds

while true do    
    
    inpto	= inpt;
	inpt	= input.get()
    
	if  inpt[keyBindings['keyMute']] and not inpto[keyBindings['keyMute']] then
        emu.mute()
	end
    
	if  inpt[keyBindings['keyUnmute']] and not inpto[keyBindings['keyUnmute']] then
        emu.unmute()
	end
    
    -- subtract base time from current; if divisible by interval, 
    current_interval = (os.time() - base_time) % save_interval

    if current_interval == 0
    and did_save == false then
        hax.save(save_path .. "\\" .. save_prefix .. os.time() .. "." .. save_ext)
        did_save = true
    elseif current_interval ~= 0 then
        did_save = false 
    end
        
        
    
    emu.frameadvance()
    
end
