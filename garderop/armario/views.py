from django.shortcuts import render
import subprocess
import json

from django.http import HttpResponse
from django.contrib.auth.decorators import login_required

def home(request):
    context = {
    }
    return render(request, 'armario/home.html', context)

def working(request):
    context = {
    }
    return render(request, 'armario/working.html', context)

def about(request):
    return render(request, 'armario/about.html', {'title': 'About'})


@login_required
def img_call(request):
    popen = subprocess.Popen(["python3","/home/pi/Desktop/project/esp32_temp.py"], stdout=subprocess.PIPE, universal_newlines=True)
    response_data = {"response":[]}
    for stdout_line in iter(popen.stdout.readline, ""):
        response_data["response"].append(stdout_line)
        
    return HttpResponse(json.dumps(response_data), content_type="application/json")

@login_required
def view_img(request):
    context = {
    }
    return render(request, 'armario/image_view.html', context)
