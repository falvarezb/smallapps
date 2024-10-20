# Whisper AI

Dockerfile used to create an environment to execute [Whisper AI](https://github.com/openai/whisper). Whisper AI may be used to transcribe audio recordings and do translations.

The environment also contains the application [ffmpeg](https://ffmpeg.org) to convert video and audio.

Build the image and run a container (the host folder containing the data must be mounted on the path `/data` of the container).

```
docker build -t whisper:1.0.0 .
docker run --rm -d --name whisper -v "$(pwd)":/data whisper:1.0.0
```

Connect to the container
```
docker exec -it whisper /bin/bash
```

and activate the Python environment with the Whisper application
```
source /opt/venv/bin/activate
```
