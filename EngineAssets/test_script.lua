

time = 0
camSpeed =5
camYawSpeed = 50
camPitchSpeed = 50


function Update(ts)
    
    time = time + ts;

    if time > 1 then
        print("Curent ts is "..math.floor(1/ts).." fps");
        time = 0
    end

    if CanEditScene then
        camVel = {
            x=0,
            y=0,
            z=0,
        };

        if GetMouseDown(Mouse.Right) then
            motion = GetMouseMotion();
            SetCameraYawVelocity(motion.x*camYawSpeed)
            SetCameraPitchVelocity(motion.y*camPitchSpeed)
        else
            SetCameraYawVelocity(0)
            SetCameraPitchVelocity(0)            
        end

        if GetKeyDown(Key.W) then
            camVel.z = camVel.z + camSpeed;
        end
        if GetKeyDown(Key.S) then
            camVel.z = camVel.z - camSpeed;
        end
        
        if GetKeyDown(Key.D) then
            camVel.x = camVel.x + camSpeed;
        end
        if GetKeyDown(Key.A) then
            camVel.x = camVel.x - camSpeed;
        end
        
        if GetKeyDown(Key.E) then
            camVel.y = camVel.y + camSpeed;
        end
        if GetKeyDown(Key.Q) then
            camVel.y = camVel.y - camSpeed;
        end
        SetCameraVelocity(camVel);

    else
        SetCameraVelocity({});
    end

end
