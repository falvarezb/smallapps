FROM python:3.12-slim

WORKDIR /data

RUN apt-get update && apt-get install -y \
    ffmpeg \  
    && rm -rf /var/lib/apt/lists/*


# https://github.com/openai/whisper
RUN python3 -m venv /opt/venv \
    && /opt/venv/bin/pip install -U openai-whisper
 
# Pre-fetch the default model
RUN /opt/venv/bin/python3 -c "import whisper; whisper.load_model('large-v3-turbo')"

ENTRYPOINT [ "tail", "-f", "/dev/null" ]
