from http.client import HTTPException

from fastapi import FastAPI
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware  # Add this import
import subprocess
from pydantic import BaseModel

app = FastAPI()

origins = [
  "http://localhost",
  "http://localhost:8000",
  "http://localhost:5173",
]

app.add_middleware(
  CORSMiddleware,
  allow_origins=origins,
  allow_credentials=True,
  allow_methods=["*"],
  allow_headers=["*"],
)


@app.get("/")
async def root():
    return {"message": "Hello World"}


@app.get("/hello/{name}")
async def say_hello(name: str):
    return {"message": f"Hello {name}"}

@app.post("/run")
async def run_code(request: Request):
    data = await request.json()
    code = data["code"]
    print(code)
    with open("code.us", "w") as f:
        f.write(code)
    try:
        print("Running subprocess...")
        result = subprocess.run(["./backend", "code.us", "workword"], capture_output=True, text=True,
                                timeout=5)
        print("Subprocess finished.")
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        return {"output": e.output, "error": e.stderr}
    return {"output": result.stdout, "error": None}