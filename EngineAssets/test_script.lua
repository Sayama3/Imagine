

global_time = 0
time = 0
camSpeed = 5
camRotSpeed = 40
player = nil
entity = nil

function UpdateCamera(ts)
    if CanEditScene then
        if GetKeyPressed(Key.Up) then
            camSpeed = math.max(camSpeed + 1, 0);
            print("camSpeed is "..camSpeed)
        end
        if GetKeyPressed(Key.Down) then
            camSpeed = math.max(camSpeed - 1, 0);
            print("camSpeed is "..camSpeed)
        end

        if GetKeyPressed(Key.Right) then
            camRotSpeed = math.max(camRotSpeed + 5, 0);
            print("camRotSpeed is "..camRotSpeed)
        end
        if GetKeyPressed(Key.Left) then
            camRotSpeed = math.max(camRotSpeed - 5, 0);
            print("camRotSpeed is "..camRotSpeed)
        end

        if GetMouseDown(Mouse.Right) then
            motion = GetMouseMotion();
            Camera.Main.yawVelocity = motion.x*camRotSpeed;
            Camera.Main.pitchVelocity = motion.y*camRotSpeed;
        else
            SetCameraYawVelocity(0)
            SetCameraPitchVelocity(0)            
        end

        Camera.Main.velocity_z = 0
        if GetKeyDown(Key.W) then
            Camera.Main.velocity_z = Camera.Main.velocity_z + camSpeed;
        end
        if GetKeyDown(Key.S) then
            Camera.Main.velocity_z = Camera.Main.velocity_z - camSpeed;
        end
        
        Camera.Main.velocity_x = 0
        if GetKeyDown(Key.D) then
            Camera.Main.velocity_x = Camera.Main.velocity_x + camSpeed;
        end
        if GetKeyDown(Key.A) then
            Camera.Main.velocity_x = Camera.Main.velocity_x - camSpeed;
        end
        
        Camera.Main.velocity_y = 0
        if GetKeyDown(Key.E) then
            Camera.Main.velocity_y = Camera.Main.velocity_y + camSpeed;
        end
        if GetKeyDown(Key.Q) then
            Camera.Main.velocity_y = Camera.Main.velocity_y - camSpeed;
        end

    else
        SetCameraVelocity({});
        Camera.Main.yawVelocity = 0;
        Camera.Main.pitchVelocity = 0;
    end
end


function Update(ts)
    
    time = time + ts;
    global_time = global_time + ts
    if time > 1 then
        print("Curent ts is "..math.floor(1/ts).." fps");
        time = 0
    end

    UpdateCamera(ts)

    if(player == nil) then
        player = FindEntityByName("Player");
    end


    if entity == nil then
        return
    end

    if not EntityExist(entity) then
        return
    end

    direction = Vec3.new(1)
    print(direction)
    if(player ~= nil) then
        if(entity ~= nil) then
            ePlayer = GetEntity(player)
            eEntity = GetEntity(entity)
            print(eEntity)
            print(ePlayer)
            print(eEntity.position)
            print(ePlayer.position)
            direction = ePlayer.position - eEntity.position   
            print(direction)
            Vec3.Normalize(direction);
            print(direction)
        end
    end

    physics = GetOrAddPhysics(entity)
    if direction ~= Vec3.new(0) then
        physics.linear_velocity = direction * Vec3.new(ts);
    end
    -- SetPhysics(entity, physics)
end

function SetEntity(enttId)
    entity = enttId
end